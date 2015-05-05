/**
 * @author Takuji Kawata
 * Updated 2015/05/05
 */

#include "audioproxy.h"
#include "NAOqi/nao_mic_interface/nao_mic_interface.h"

#include <QDebug>
#include <QtCore/qendian.h>


MicToNAOqiAudioProxy::MicToNAOqiAudioProxy(const QAudioFormat &format, QObject *parent, bool mute)
    :   QIODevice(parent)
    ,   m_format(format)
    ,   m_maxAmplitude(0)
    ,   m_level(0.0)
    ,   m_muted(mute)
    ,   m_buffer(0)
{

    //TODO : only sample size 16 with SignedInt is supported now.

    switch (m_format.sampleSize()) {
    case 8:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            qWarning() << "sample size 8 is not supported!!";
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            qWarning() << "sample size 8 is not supported!!";
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            qWarning() << "sample size 16 unsiged int is not supported!!";
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    m_buffer = new signed short[BUFFERSAMPLESIZE * NBOFOUTPUTCHANNELS];

}

MicToNAOqiAudioProxy::~MicToNAOqiAudioProxy()
{
    if (m_workerThread)
        delete m_workerThread;

    if (m_buffer)
        delete m_buffer;
    m_buffer = NULL;

    delete m_workerThread;
}

void MicToNAOqiAudioProxy::start()
{
    m_workerThread = new MicToNAOqiWorkerThread(m_muted);
    m_workerThread->start();

    open(QIODevice::WriteOnly);
}

void MicToNAOqiAudioProxy::stop()
{
    delete m_workerThread;
    m_workerThread = NULL;
    close();
}

qint64 MicToNAOqiAudioProxy::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

void MicToNAOqiAudioProxy::setMute(bool mute)
{
    m_muted = mute;
    if (m_muted)
    {
        m_level = 0;
        emit levelUpdated();
    }
    m_workerThread->setMute(mute);
}

qint64 MicToNAOqiAudioProxy::writeData(const char *data, qint64 len)
{
    if (m_muted)
    {
        return len;
    }

    if (len > BUFFERSAMPLESIZE * NBOFOUTPUTCHANNELS * CHANNELBYTES)
    {
        qWarning() << "Audio write data exceeded the buffer size!!" << len << "  -->  " <<  BUFFERSAMPLESIZE * NBOFOUTPUTCHANNELS * CHANNELBYTES;
        len = BUFFERSAMPLESIZE * NBOFOUTPUTCHANNELS * CHANNELBYTES;
    }

    if (m_maxAmplitude) {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);
        const int channelBytes = m_format.sampleSize() / 8;
        const int sampleBytes = m_format.channels() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        int maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);
        int p = 0;

        for (int i = 0; i < numSamples; ++i) {
            for(int j = 0; j < m_format.channels(); ++j) {
                int value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    value = *reinterpret_cast<const quint8*>(ptr);
                } else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    value = *reinterpret_cast<const qint8*>(ptr);
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<qint16>(ptr);
                    else
                        value = qFromBigEndian<qint16>(ptr);
                }

                m_buffer[p++] = value;

                maxValue = qMax(qAbs(value), maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, (int) m_maxAmplitude);
        m_level = qreal(maxValue) / m_maxAmplitude;
        if (m_workerThread)
        {
            m_workerThread->writeAudioBuffer(m_buffer, numSamples);
        }
    }

    emit levelUpdated();
    return len;
}

MicToNAOqiWorkerThread::MicToNAOqiWorkerThread(bool mute) : m_quit(false), m_muted(mute)
{
    m_buffers[0] = new signed short[BUFFERSAMPLESIZE * NBOFOUTPUTCHANNELS];
    m_buffers[1] = new signed short[BUFFERSAMPLESIZE * NBOFOUTPUTCHANNELS];
    m_numSamples[0] = 0;
    m_numSamples[1] = 0;

    m_naoInterface = NaoMicInterface::instance();
}

MicToNAOqiWorkerThread::~MicToNAOqiWorkerThread()
{
    m_quit = true;
    m_sem.release();
    wait(5000);

    for (int i = 0; i < 2; i++)
    {
        if (m_buffers[i])
        {
            delete m_buffers[i];
            m_buffers[i] = NULL;
        }
    }
}

void MicToNAOqiWorkerThread::run()
{
    while(!m_quit)
    {
        m_sem.acquire();
        if (!m_quit)
        {
            if (m_naoInterface && m_naoInterface->isConnected())
            {
                if (!m_muted)
                {
                    memcpy(m_buffers[1], m_buffers[0], m_numSamples[1]* NBOFOUTPUTCHANNELS * CHANNELBYTES);
                    m_numSamples[1] = m_numSamples[0];
                    m_naoInterface->writeAudioBuffer(m_buffers[1], m_numSamples[1]);
                }
                else
                {
                    qWarning() << "muting.. clearing buffer";
                    m_naoInterface->clearBuffer();
                    m_numSamples[0] = 0;
                    m_numSamples[1] = 0;
                }
            }
        }
    }

}


void MicToNAOqiWorkerThread::writeAudioBuffer(signed short *buffer, int numSamples)
{
    memcpy(m_buffers[0], buffer, numSamples*NBOFOUTPUTCHANNELS*CHANNELBYTES);
    m_numSamples[0] = numSamples;
    if (m_sem.available()>0)
    {
        //qWarning() << "buffer delays..";
        return;
    }
    m_sem.release();
}

void MicToNAOqiWorkerThread::setMute(bool mute)
{
    m_muted = mute;
    if (m_muted)
    {
        m_sem.release();
    }
}

