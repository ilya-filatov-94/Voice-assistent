#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QMovie>

#include "audiorecorder.h"
#include "networkrequests.h"
#include "sliderbutton.h"
#include "commandsexec.h"
#include "recogniespeechyandexapi.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget* widgetWindow;
    QVBoxLayout* vertical_layout;
    QPushButton* btn_commandList;
    QPushButton* toggle_record;
    QMovie* movie;
    QLabel* header_textArea;
    QTextEdit* textArea;
    QPushButton* btn_clearTextArea;
    QLabel* labelMode;
    SliderButton* btn_modeSelection;
    QHBoxLayout* layout_mode;

    AudioRecorder* recorder;
    bool statusRecord;
    NetworkRequests* request_manager;
    QString modelRecognition;

    CommandsExec* assistant;


    RecognizedSpeechYandexAPI* yaSpeech;

public slots:

    void getPathToAudioFile(QString);
    void updateStatusProcess(QString);
    void getRecognizedSpeech(QString);

private slots:
    void startRecord();
    void setButtonIcon();
    void clearTextArea();
    void changingStatusSlider(bool);

signals:
    void commandRecord(bool);
    void startRequestsToAPI(QString, QString);
    void requestCommand(QString);

    void postYaAPI(QString);

};

#endif // MAINWINDOW_H
