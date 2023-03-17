#include "recogniespeechyandexapi.h"

RecognizedSpeechYandexAPI::RecognizedSpeechYandexAPI(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &RecognizedSpeechYandexAPI::onResult, Qt::QueuedConnection);
}

RecognizedSpeechYandexAPI::~RecognizedSpeechYandexAPI()
{
    delete manager;
}

void RecognizedSpeechYandexAPI::postAudioToAPI(QString filePath)
{
    QString uploadUrl = "https://stt.api.cloud.yandex.net/speech/v1/stt:recognize";
    filePath += ".wav";
//    QFile file("14a76108-1cbf-4f0b-99f2-6a53549adf30_.ogg");
    QFile file("my_voice.mp3");
    QString fileName = filePath.section("/",-1,-1);

    if(file.open(QIODevice::ReadOnly)) {

        QByteArray dataFile = file.readAll();
        QNetworkRequest request;
        request.setRawHeader("Authorization", QString("Api-Key %1").arg(secretYandexKey).toUtf8());
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

        QUrl url(uploadUrl);
        request.setUrl(url);
        manager->post(request, dataFile);
    }
    else {
        return;
    }
}

void RecognizedSpeechYandexAPI::onResult(QNetworkReply *reply)
{
    if (!reply->error()) {
        QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
        qDebug() << "Распознанная речь: " << data["result"].toString();
    }
    else {
        qDebug() << QJsonDocument::fromJson(reply->readAll());
    }
}
