#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QObject>
#include <QAudioProbe>
#include <QAudioRecorder>
#include <QAudioBuffer>
#include <QMediaRecorder>
#include <QSettings>
#include <QUrl>
#include <QStandardPaths>
#include <QUuid>
#include <QDir>
#include <QTimer>

class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorder(QObject *parent = nullptr);
    ~AudioRecorder();

private:
    QAudioRecorder* audioRecorder;
    QAudioProbe* m_probe;
    QUrl url;
    QString filePath;
    QFile temporaryFile;
    const QDir location = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    qreal thresholdOfAudioinputActivation = 5.0;
    bool statusSpeech;
    bool newStatusSpeech;
    QTimer timerOfFinishedSpeech;

    const int intervalRecording = 15000;
    QTimer voiceCommandWaitTimer;
    QString modeAudioInput;

signals:

    void sendPathToAudioFile(QString);

public slots:

    void toggleRecord(bool);
    void processBuffer(const QAudioBuffer&);
    void timeoutOfSpeech();
    void restartWaitTimer();
};

#endif // AUDIORECORDER_H
