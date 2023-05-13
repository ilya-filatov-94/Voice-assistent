#include "yandexspeechrecognition.h"

YandexSpeechRecognition::YandexSpeechRecognition(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &YandexSpeechRecognition::onResult, Qt::QueuedConnection);
    initListOfErrors();
}

YandexSpeechRecognition::~YandexSpeechRecognition()
{
    delete manager;
}

void YandexSpeechRecognition::postAudioToAPI(QString filePath)
{
    QString uploadUrl = "https://stt.api.cloud.yandex.net/speech/v1/stt:recognize";
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray dataFile = file.readAll();
        file.close();
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

void YandexSpeechRecognition::onResult(QNetworkReply *reply)
{
    bool error = false;
    QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
    if (reply->error()) {
        error = true;
        if (listOfErrors.contains(data["error_code"].toString())) {
            sendStatusError(listOfErrors.value(data["error_code"].toString()));
        }
        else {
            sendStatusError(data["error_message"].toString());
        }
    }

    if (!error) {
        sendRecognizedSpeech(data["result"].toString());
    }
}

void YandexSpeechRecognition::setYandexSpeechToken(QString token)
{
    secretYandexKey = token;
}

void YandexSpeechRecognition::initListOfErrors() {
    listOfErrors.insert("BAD_REQUEST", "Синтаксическая ошибка в запросе к API распознавания речи");
    listOfErrors.insert("UNAUTHORIZED", "Ошибка авторизации в API распознавания речи");
    listOfErrors.insert("TOO_MANY_REQUESTS", "Превышен лимит запросов к API распознавания речи");
    listOfErrors.insert("INTERNAL_SERVER_ERROR", "Внутренние ошибки API распознавания речи");
}
