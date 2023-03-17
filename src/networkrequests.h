#ifndef NETWORKREQUESTS_H
#define NETWORKREQUESTS_H

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

class NetworkRequests : public QObject
{
    Q_OBJECT
public:
    explicit NetworkRequests(QObject *parent = nullptr);
    ~NetworkRequests();

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

public slots:

    void requestUploadUrlAPI(QString, QString);

private slots:

    void onResult(QNetworkReply* reply);
    QString getUploadUrlAPI(QNetworkReply*);
    void uploadAudioToServer(QString);
    void getIDforAudioProcessing(QJsonDocument);
    void startAudioRecognitionProcess(QString);
    void timeout_timerOfRetryRequests();

};

#endif // NETWORKREQUESTS_H
