#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QMovie>
#include <QMessageBox>
#include <QSqlDatabase>

#include "audiorecorder.h"
#include "vkasrspeechrecognition.h"
#include "sliderbutton.h"
#include "commandsexec.h"
#include "yandexspeechrecognition.h"
#include "datamapper.h"
#include "networkaccess.h"
#include "settingswindow.h"


#include <QDebug>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void readData();

private:
    QWidget* widgetWindow;
    QVBoxLayout* vertical_layout;
    QPushButton* btn_settings;
    QPushButton* toggle_record;
    QMovie* movie;
    QLabel* header_textArea;
    QTextEdit* textArea;
    QPushButton* btn_clearTextArea;

    QLabel* labelServiceRecognition;
    SliderButton* btnServiceRecognition;
    QHBoxLayout* layout_service;

    QLabel* labelMode;
    SliderButton* btn_modeSelection;
    QHBoxLayout* layout_mode;
    QString serviceRecognition;

    AudioRecorder* recorder;
    bool statusRecord;
    VkASRspeechRecognition* VkSpeechRec;
    QString modelRecognition;
    YandexSpeechRecognition* yaSpeechRec;
    CommandsExec* assistant;
    QSqlDatabase db;
    Datamapper* dataMapper;
    NetworkAccess* networkAccess;
    SettingsWindow* setingsWindow;

    void createUserInterface();
    void showMessageBoxErrorCommand(QString&);
    void showSettings(bool);

public slots:

    void getPathToAudioFile(QString);
    void updateStatusProcess(QString);
    void speechRecognitionError(QString);
    void getRecognizedSpeech(QString);
    void errorInternetConnection(bool);

private slots:

    void setButtonIcon();
    void changingServiceSlider(bool);
    void changingModeSlider(bool);
    void startRecord();
    void clearTextArea();
    void showSettingsWindow();

signals:

    void sendPathToFFMPEG(QString);
    void sendVkSpeechToken(QString);
    void sendYandexSpeechToken(QString);
    void sendYandexGeoToken(QString);
    void sendSettingsData(QString, QString, QString, QString);

    void commandRecord(bool);
    void setServiceRecognition(QString);
    void postRequestToYandexAPI(QString);
    void postRequestToVkAPI(QString, QString);
    void requestCommand(QString);

};

#endif // MAINWINDOW_H
