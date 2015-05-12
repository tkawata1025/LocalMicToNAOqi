/**
 * @author Takuji Kawata
 * Updated 2015/05/05
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NAOqi/nao_mic_interface/nao_mic_interface.h"

#include <QMutex>
#include <QQueue>
#include <QtCore/qendian.h>
#include <QPainter>
#include <QDebug>

static bool             s_isConnected = false;
static QMutex           s_consoleMutex;
static QQueue<QString>  s_pendingConsoleMessages;
static MainWindow       *s_window = NULL;

const int QTAUDIOBUFFERSIZE = 5000;

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_level = 0;
    setMinimumHeight(30);
    setMinimumWidth(200);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.drawRect(QRect(painter.viewport().left()+10,
                           painter.viewport().top()+10,
                           painter.viewport().right()-20,
                           painter.viewport().bottom()-20));
    if (m_level == 0.0)
        return;

    painter.setPen(Qt::red);

    int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_level;
    for (int i = 0; i < 10; ++i) {
        int x1 = painter.viewport().left()+11;
        int y1 = painter.viewport().top()+10+i;
        int x2 = painter.viewport().left()+20+pos;
        int y2 = painter.viewport().top()+10+i;
        if (x2 < painter.viewport().left()+10)
            x2 = painter.viewport().left()+10;

        painter.drawLine(QPoint(x1, y1),QPoint(x2, y2));
    }
}

void RenderArea::setLevel(qreal value)
{
    m_level = value;
    repaint();
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audioProxy(0),
    m_audioInput(0),
    m_device(QAudioDeviceInfo::defaultInputDevice()),
    m_naoInterface(0),
    m_volumePressed(false)
{
    // set up UI
    ui->setupUi(this);
    ui->disconnectButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->console->setMaximumBlockCount(100);
    ui->console->setReadOnly(true);
    QFont monofont("Courier");
    monofont.setStyleHint(QFont::Monospace);
    ui->console->setFont(monofont);
    m_canvas = new RenderArea(this);
    ui->soundLevel->addWidget(m_canvas);
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i)
        ui->deviceBox->addItem(devices.at(i).deviceName(), QVariant::fromValue(devices.at(i)));

    // set up NAO interface
    m_naoInterface = NaoMicInterface::instance();

    // Set up audio
    m_format.setSampleRate(SAMPLERATE);
    m_format.setChannels(NBOFOUTPUTCHANNELS);
    m_format.setSampleSize(16);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(m_format)) {
        m_format = info.nearestFormat(m_format);
    }
    qWarning() << "sample rate:" << m_format.sampleRate() << "  channels:" << m_format.channels() << "  sample size:" <<  m_format.sampleSize();

    m_audioProxy  = new MicToNAOqiAudioProxy(m_format, this, !ui->micButton->isChecked());
    connect(m_audioProxy, SIGNAL(levelUpdated()), SLOT(refreshDisplay()));

    startAudioInput();

    // Set up UI event connections
    connect(ui->deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    connect(this, SIGNAL(consoleUpdated()), this, SLOT(update()), Qt::AutoConnection);
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
    connect(ui->disconnectButton, SIGNAL(clicked()), this, SLOT(disconnectButtonClicked()));
    connect(ui->micButton, SIGNAL(clicked()), this, SLOT(micButtonClicked()));
    connect(ui->outputVolume, SIGNAL(sliderPressed()) , this, SLOT(outputVolumePressed()));
    connect(ui->outputVolume, SIGNAL(sliderReleased()), this, SLOT(outputVolumeReleased()));

    // Set up timer interval for output volume
    m_outputVolumeTimer = new QTimer(this);
    connect(m_outputVolumeTimer, SIGNAL(timeout()), this, SLOT(updateOutputVolume()));
    m_outputVolumeTimer->start(1000);

    //
    s_window = this;
}

MainWindow::~MainWindow()
{
    m_audioProxy->stop();
    m_naoInterface->disconnect();
    s_window = NULL;
    delete ui;
}

void MainWindow::startAudioInput()
{
    m_audioInput = new QAudioInput(m_device, m_format, this);
    m_audioInput->setBufferSize(QTAUDIOBUFFERSIZE);
    connect(m_audioInput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    m_audioProxy->start();
    m_audioInput->start(m_audioProxy);
}

void MainWindow::stopAudioInput()
{
    m_audioProxy->stop();
    m_audioInput->stop();
    m_audioInput->disconnect(this);
    delete m_audioInput;
}


void MainWindow::notified()
{
//    qWarning() << "bytesReady = " << m_audioInput->bytesReady()
//               << ", " << "elapsedUSecs = " <<m_audioInput->elapsedUSecs()
//               << ", " << "processedUSecs = "<<m_audioInput->processedUSecs();
}


//static
void MainWindow::consoleMessage(const QString &msg)
{
    QMutexLocker lock(&s_consoleMutex);

    bool hasPendingMessage = s_pendingConsoleMessages.length() > 0;
    s_pendingConsoleMessages.append(msg);
    if (s_window)
    {
        if (hasPendingMessage)
        {
            emit s_window->consoleUpdated();
        }
    }
}

void MainWindow::connectButtonClicked()
{
    s_isConnected = false;
    stopAudioInput();
    try
    {
        QString msg = "connecting to ";
        msg.append(ui->naoIp->text());
        msg.append("...");
        ui->console->appendPlainText(msg);
        m_naoInterface->setNaoIp(ui->naoIp->text().toStdString());
        ui->console->appendPlainText("connected!");
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);
        ui->naoIp->setReadOnly(true);
        s_isConnected = true;
    }
    catch ( std::string exceptionMsg )
    {
        ui->console->appendPlainText("ERROR: connction failed.");
        QString msg = QString::fromStdString(exceptionMsg);
        ui->console->appendPlainText(msg);
    }
    startAudioInput();
}

void MainWindow::disconnectButtonClicked()
{
    try
    {
        QString msg = "disconnect from ";
        msg.append(ui->naoIp->text());
        msg.append("...");
        ui->console->appendPlainText(msg);
        m_naoInterface->disconnect();
        ui->console->appendPlainText("disconnected!");
        ui->connectButton->setEnabled(true);
        ui->disconnectButton->setEnabled(false);
        ui->naoIp->setReadOnly(false);
        s_isConnected = false;
    }
    catch ( std::string exceptionMsg )
    {
        ui->console->appendPlainText("ERROR: connction failed.");
        QString msg = QString::fromStdString(exceptionMsg);
        ui->console->appendPlainText(msg);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMutexLocker lock(&s_consoleMutex);

    while (!s_pendingConsoleMessages.isEmpty())
    {
        ui->console->appendPlainText(s_pendingConsoleMessages.dequeue());
    }
    QMainWindow::paintEvent(event);
}

void MainWindow::stateChanged(QAudio::State state)
{
    qWarning() << "state = " << state;
    if (state == QAudio::IdleState)
    {
        qWarning() << "force reactivate audio input..";
        stopAudioInput();
        startAudioInput();
    }
}

void MainWindow::refreshDisplay()
{
    m_canvas->setLevel(m_audioProxy->level());
    m_canvas->repaint();
}


void MainWindow::deviceChanged(int index)
{
    stopAudioInput();
    m_device = ui->deviceBox->itemData(index).value<QAudioDeviceInfo>();
    startAudioInput();
}

void MainWindow::micButtonClicked()
{
    m_audioProxy->setMute(!ui->micButton->isChecked());
}

void MainWindow::updateOutputVolume()
{
    if (m_volumePressed) return;

    int v = 0;
    if (m_naoInterface->isConnected())
    {
        v = m_naoInterface->getSpeakerVolume();
    }
    ui->outputVolume->setValue(v);
}

void MainWindow::outputVolumePressed()
{
    m_volumePressed = true;
}

void MainWindow::outputVolumeReleased()
{
    if (m_naoInterface->isConnected())
    {
        int v = ui->outputVolume->value();
        m_naoInterface->setSpeakerVolume(v);
    }
    m_volumePressed = false;
}
