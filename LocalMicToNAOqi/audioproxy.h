/**
 * @author Takuji Kawata
 * Updated 2015/05/05
 */

#ifndef AUDIOPROXY_H
#define AUDIOPROXY_H

#include <qaudioinput.h>
#include <QThread>
#include <QSemaphore>

class NaoMicInterface;

class MicToNAOqiWorkerThread;

class MicToNAOqiAudioProxy : public QIODevice
{
    Q_OBJECT

public:
    MicToNAOqiAudioProxy(const QAudioFormat &format, QObject *parent, bool mute);
    ~MicToNAOqiAudioProxy();

    void    start();
    void    stop();

    void    setMute(bool mute);
    bool    getMute() const { return m_muted; }

    qreal   level() const { return m_level; }

    qint64  readData(char *data, qint64 maxlen);
    qint64  writeData(const char *data, qint64 len);

signals:
    void    levelUpdated();

private:
    const QAudioFormat  m_format;
    quint16             m_maxAmplitude;
    qreal               m_level; // 0.0 <= m_level <= 1.0

    bool                m_muted;
    signed short        *m_buffer;
    MicToNAOqiWorkerThread *m_workerThread;

};

class MicToNAOqiWorkerThread : public QThread
{
    Q_OBJECT

public:
    MicToNAOqiWorkerThread(bool mute);
    virtual ~MicToNAOqiWorkerThread();

    void    run();
    void    writeAudioBuffer(signed short *buffer, int numSamples);
    void    setMute(bool mute);
    bool    getMute() const { return m_muted; }

private:
    bool                m_quit;
    bool                m_muted;
    signed short        *m_buffers[2];
    int                 m_numSamples[2];
    NaoMicInterface     *m_naoInterface;
    QSemaphore          m_sem;

};

#endif // AUDIOPROXY_H
