#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QObject>
#include <QAudioRecorder>
#include <QUrl>
#include <QUuid>
#include <QDir>
#include <QTimer>
#include <QProcess>
#include "audioprobedevice.h"


class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorder(QObject *parent = nullptr);
    ~AudioRecorder();
    void deleteAudioFiles();

private:
    QAudioRecorder* audioRecorder;
    QUrl url;
    QString filePath;
    QString fileName;
    QVector<QString> listAudioFiles;

    qreal thresholdOfAudioinputActivation;
    bool statusSpeech;
    bool newStatusSpeech;
    bool statusTrigger;
    QTimer timerOfFinishedSpeech;

    QString serviceRecognition;
    QString audioConversionProcess;
    QProcess* processConvert;

    AudioProbeDevice* audioProbe;
    QAudioInput* audioSource;

    void setAudioRecorderSettings();
    void setAudioBufferSettings();
    void audioTrigger(bool);
    void audioConversion();

signals:

    void sendPathToAudioFile(QString);
    void sendStatusProcess(QString);

public slots:

    void toggleRecord(bool);
    void changeServiceRecognition(QString);
    void setPathToFFMPEG(QString);

private slots:

    void processBuffer(QAudioBuffer);
    void timeoutOfSpeech();
    void resultProcess(int, QProcess::ExitStatus);
};

#endif // AUDIORECORDER_H
