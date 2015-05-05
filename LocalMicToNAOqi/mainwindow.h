/**
 * @author Takuji Kawata
 * Updated 2015/05/05
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QThread>
#include <QSemaphore>

#include <qaudioinput.h>

#include "audioproxy.h"

namespace Ui {
class MainWindow;
}

class NaoMicInterface;

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    void setLevel(qreal value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    qreal m_level;
    QPixmap m_pixmap;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void consoleMessage(const QString& msg);

private:
    Ui::MainWindow *ui;

protected:
    virtual void paintEvent(QPaintEvent *event );

signals:
    void consoleUpdated();

private:
    MicToNAOqiAudioProxy    *m_audioProxy;
    QAudioFormat            m_format;
    QAudioInput             *m_audioInput;
    QAudioDeviceInfo        m_device;
    RenderArea              *m_canvas;
    QTimer                  *m_outputVolumeTimer;

    NaoMicInterface         *m_naoInterface;
    bool                    m_volumePressed;

    void startAudioInput();
    void stopAudioInput();

private slots:
    void connectButtonClicked();
    void disconnectButtonClicked();
    void updateOutputVolume();
    void refreshDisplay();
    void notified();
    void stateChanged(QAudio::State state);
    void deviceChanged(int index);
    void micButtonClicked();
    void outputVolumePressed();
    void outputVolumeReleased();
};

#endif // MAINWINDOW_H
