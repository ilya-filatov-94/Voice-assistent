#ifndef YANDEXSPEECHRECOGNITION_H
#define YANDEXSPEECHRECOGNITION_H


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

class YandexSpeechRecognition : public QObject
{
    Q_OBJECT
public:
    explicit YandexSpeechRecognition(QObject *parent = nullptr);
    ~YandexSpeechRecognition();

private:

    QNetworkAccessManager* manager;
    QHttpMultiPart* multiPart;
    QString secretYandexKey;

    void initListOfErrors();
    QMap<QString, QString> listOfErrors;

signals:
    void sendRecognizedSpeech(QString);
    void sendStatusError(QString);

public slots:
    void postAudioToAPI(QString);
    void setYandexSpeechToken(QString);

private slots:
    void onResult(QNetworkReply* reply);
};

#endif // YANDEXSPEECHRECOGNITION_H
