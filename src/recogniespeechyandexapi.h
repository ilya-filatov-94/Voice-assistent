#ifndef RECOGNIESPEECHYANDEXAPI_H
#define RECOGNIESPEECHYANDEXAPI_H

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

class RecognizedSpeechYandexAPI : public QObject
{
    Q_OBJECT
public:
    explicit RecognizedSpeechYandexAPI(QObject *parent = nullptr);
    ~RecognizedSpeechYandexAPI();

private:

    QNetworkAccessManager* manager;
    QHttpMultiPart* multiPart;
    //OAuth-token
    QString tokenYaSpeechKit;
    //IAM token
    QString iamTokenYandex;
    //Service-key
    QString secretYandexKey;


signals:

public slots:
    void postAudioToAPI(QString);

private slots:
    void onResult(QNetworkReply* reply);
};

#endif // RECOGNIESPEECHYANDEXAPI_H
