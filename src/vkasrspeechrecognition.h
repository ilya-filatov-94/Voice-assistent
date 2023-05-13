#ifndef VKASRSPEECHRECOGNITION_H
#define VKASRSPEECHRECOGNITION_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QMap>

class VkASRspeechRecognition : public QObject
{
    Q_OBJECT
public:
    explicit VkASRspeechRecognition(QObject *parent = nullptr);
    ~VkASRspeechRecognition();

private:
    QNetworkAccessManager* manager;
    QHttpMultiPart* multiPart;
    QString serviceKey;
    QString filePath;
    int requestProcessingStep;
    QString task_id;
    QTimer timerOfRetryRequests;
    QMap<int, QString> listOfError;
    QMap<QString, QString> listOfStatus;
    void initListOfErrors();
    QString modelRecognition;

signals:

    void sendRecognizedSpeech(QString);
    void sendStatusProcess(QString);
    void sendStatusError(QString);

public slots:

    void requestUploadUrlAPI(QString, QString);
    void setVkSpeechToken(QString);

private slots:

    void onResult(QNetworkReply* reply);
    QString getUploadUrlAPI(QNetworkReply*);
    void uploadAudioToServer(QString);
    void getIDforAudioProcessing(QJsonDocument);
    void startAudioRecognitionProcess(QString);
    void timeout_timerOfRetryRequests();

};

#endif // VKASRSPEECHRECOGNITION_H
