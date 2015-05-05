/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue May 5 23:02:51 2015
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPlainTextEdit *console;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QLineEdit *naoIp;
    QLabel *label;
    QComboBox *deviceBox;
    QPushButton *micButton;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *soundLevel;
    QSlider *outputVolume;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(614, 311);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        console = new QPlainTextEdit(centralWidget);
        console->setObjectName(QString::fromUtf8("console"));
        console->setGeometry(QRect(329, 30, 271, 211));
        connectButton = new QPushButton(centralWidget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(344, -4, 94, 32));
        disconnectButton = new QPushButton(centralWidget);
        disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));
        disconnectButton->setGeometry(QRect(230, -4, 112, 32));
        naoIp = new QLineEdit(centralWidget);
        naoIp->setObjectName(QString::fromUtf8("naoIp"));
        naoIp->setGeometry(QRect(110, 0, 116, 21));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(3, 0, 99, 21));
        deviceBox = new QComboBox(centralWidget);
        deviceBox->setObjectName(QString::fromUtf8("deviceBox"));
        deviceBox->setGeometry(QRect(10, 30, 281, 26));
        micButton = new QPushButton(centralWidget);
        micButton->setObjectName(QString::fromUtf8("micButton"));
        micButton->setGeometry(QRect(0, 60, 301, 141));
        QFont font;
        font.setPointSize(18);
        micButton->setFont(font);
        micButton->setCheckable(true);
        verticalLayoutWidget_3 = new QWidget(centralWidget);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(8, 200, 281, 31));
        soundLevel = new QVBoxLayout(verticalLayoutWidget_3);
        soundLevel->setSpacing(6);
        soundLevel->setContentsMargins(11, 11, 11, 11);
        soundLevel->setObjectName(QString::fromUtf8("soundLevel"));
        soundLevel->setContentsMargins(0, 0, 0, 0);
        outputVolume = new QSlider(centralWidget);
        outputVolume->setObjectName(QString::fromUtf8("outputVolume"));
        outputVolume->setGeometry(QRect(300, 70, 22, 121));
        outputVolume->setMaximum(100);
        outputVolume->setOrientation(Qt::Vertical);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 614, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("MainWindow", "Connect", 0, QApplication::UnicodeUTF8));
        disconnectButton->setText(QApplication::translate("MainWindow", "Disconnect", 0, QApplication::UnicodeUTF8));
        naoIp->setText(QApplication::translate("MainWindow", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "NAO IP address", 0, QApplication::UnicodeUTF8));
        micButton->setText(QApplication::translate("MainWindow", "\343\203\236\343\202\244\343\202\257\343\203\255\343\203\225\343\202\251\343\203\263", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
