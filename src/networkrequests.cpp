#include "networkrequests.h"

NetworkRequests::NetworkRequests(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &NetworkRequests::onResult, Qt::QueuedConnection);
    connect(&timerOfRetryRequests, &QTimer::timeout, this, &NetworkRequests::timeout_timerOfRetryRequests);
    initListOfErrors();
}

NetworkRequests::~NetworkRequests()
{
    delete manager;
}

void NetworkRequests::requestUploadUrlAPI(QString path, QString mode)
{
    requestProcessingStep = 0;
    filePath = path;
    modelRecognition = mode;
    QString apiUrl("https://api.vk.com/method/asr.getUploadUrl?");
    QNetworkRequest request;
    QUrl url(apiUrl);
    QUrlQuery query(url.query());
    query.addQueryItem("access_token", serviceKey);
    query.addQueryItem("v", "5.131");
    url.setQuery(query);
    request.setUrl(url);
    manager->get(request);
}

QString NetworkRequests::getUploadUrlAPI(QNetworkReply* reply)
{
    auto data = QJsonDocument::fromJson(reply->readAll());
    return data["response"]["upload_url"].toString();
}

void NetworkRequests::uploadAudioToServer(QString uploadUrl)
{
    filePath += ".wav";
    QFile file(filePath);
    QString fileName = filePath.section("/",-1,-1);

//    fileName = fileName.remove(fileName.size()-4, 4);
//    qDebug() << file.rename(fileName + ".ogg");

    if(file.open(QIODevice::ReadOnly)) {
        multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart fileDataPart;
        QString headerStr = QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileName);
        fileDataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(headerStr));
        fileDataPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
        fileDataPart.setBody(file.readAll());
        multiPart->append(fileDataPart);

        QUrl url(uploadUrl);
        QNetworkRequest request(url);
        manager->post(request, multiPart);
        file.close();
        file.remove();
    }
    else {
        sendStatusProcess("Ошибка открытия файла");
        return;
    }
}

void NetworkRequests::getIDforAudioProcessing(QJsonDocument audioData)
{
    QString apiUrl("https://api.vk.com/method/asr.process?");
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QUrl url(apiUrl);
    QUrlQuery query(url.query());
    query.addQueryItem("model", modelRecognition);
    query.addQueryItem("access_token", serviceKey);
    query.addQueryItem("v", "5.131");
    query.addQueryItem("audio", audioData.toJson(QJsonDocument::JsonFormat::Compact));
    url.setQuery(query);
    request.setUrl(url);
    manager->get(request);
}


void NetworkRequests::startAudioRecognitionProcess(QString taskID)
{
    QString apiUrl("https://api.vk.com/method/asr.checkStatus?");
    QNetworkRequest request;
    QUrl url(apiUrl);
    QUrlQuery query(url.query());
    query.addQueryItem("task_id", taskID);
    query.addQueryItem("access_token", serviceKey);
    query.addQueryItem("v", "5.131");
    url.setQuery(query);
    request.setUrl(url);
    manager->get(request);
}

void NetworkRequests::timeout_timerOfRetryRequests()
{
    startAudioRecognitionProcess(task_id);
}

void NetworkRequests::onResult(QNetworkReply *reply) {

    QJsonDocument data;
    bool error = false;

    if (reply->error()) {
        error = true;
        timerOfRetryRequests.stop();
        sendStatusProcess("Ошибка запроса к API");
    }

    if (!reply->error() && requestProcessingStep != 0) {
        data = QJsonDocument::fromJson(reply->readAll());
        if (listOfError.contains(data["error"]["error_code"].toInt())) {
            error = true;
            timerOfRetryRequests.stop();
            sendStatusProcess(listOfError.value(data["error"]["error_code"].toInt()));
        }
    }

    if (!reply->error() && !error) {
        switch (requestProcessingStep) {
            case 0: {
                QString uploadUrlFile = getUploadUrlAPI(reply);        //1 - получить адрес сервера для загрузки
                if (uploadUrlFile == "") {
                    sendStatusProcess("ошибка при запросе Url-адреса API");
                }
                else {
                    ++requestProcessingStep;
                    uploadAudioToServer(uploadUrlFile);                //2 - загрузить аудиозапись по полученному url API
                }
                break;
            }
            case 1: {
                ++requestProcessingStep;
                getIDforAudioProcessing(data);                        //3 - Получить id задачи на обработку аудио от API
                break;
            }
            case 2: {
                task_id = data["response"]["task_id"].toString();
                ++requestProcessingStep;
                startAudioRecognitionProcess(task_id);               //4 - Запустить процесс распознавания аудио на сервере
                break;
            }
            case 3: {
                    if (listOfStatus.contains(data["response"]["status"].toString())) {
                        timerOfRetryRequests.stop();
                        sendStatusProcess(listOfStatus.value(data["response"]["status"].toString()));
                    }

                    if (data["response"]["status"].toString() == "processing") {
                        timerOfRetryRequests.setInterval(500);
                        timerOfRetryRequests.start();
                        sendStatusProcess("выполняется процесс распознавания речи, подождите");
                    }

                    if (data["response"]["status"].toString() == "finished") {
                        timerOfRetryRequests.stop();
                        sendRecognizedSpeech(data["response"]["text"].toString());
                    }
                    break;
            }
        }
    }
}


void NetworkRequests::initListOfErrors() {

    listOfError.insert(1, "Произошла неизвестная ошибка. \r\n"
                          "Повторите команду");

    listOfError.insert(2, "Приложение API выключено \r\n"
                          "Настройте API распознавания речи");

    listOfError.insert(3, "Передан неизвестный метод \r\n"
                          "Проверьте запрос к API");

    listOfError.insert(4, "Неверная подпись, token");

    listOfError.insert(5, "Авторизация приложения в сервисе API не удалась");

    listOfError.insert(6, "Слишком много запросов в секунду к API");

    listOfError.insert(7, "Нет прав для выполнения этого действия. \r\n"
                          "Требуется авторизация в API");

    listOfError.insert(8, "Неверный синтаксис запроса к API");

    listOfError.insert(9, "Слишком много однотипных действий с API");

    listOfError.insert(10, "Произошла внутренняя ошибка сервера API");

    listOfError.insert(11, "Отключите тестовый режим API по API_ID");

    listOfError.insert(14, "Требуется ввод кода с картинки (Captcha)");

    listOfError.insert(15, "Нет доступа к данному методу API");

    listOfError.insert(16, "Требуется выполнение запросов по протоколу HTTPS");

    listOfError.insert(17, "Требуется валидация пользователя");

    listOfError.insert(18, "Страница удалена или заблокирована");

    listOfError.insert(20, "Данное действие запрещено для \r\n"
                           "не Standalone приложений");

    listOfError.insert(21, "Данное действие разрешено только для Standalone \r\n"
                           "и Open API приложений.");

    listOfError.insert(23, "Метод API был выключен");

    listOfError.insert(24, "Требуется подтверждение со стороны user API");

    listOfError.insert(27, "Ключ доступа сообщества недействителен");

    listOfError.insert(28, "Ключ доступа приложения недействителен");

    listOfError.insert(29, "Достигнут количественный лимит на вызов метода");

    listOfError.insert(30, "Профиль является приватным");

    listOfError.insert(100, "Один из необходимых параметров был не передан \r\n"
                            "или неверен.");

    listOfError.insert(101, "Неверный API ID приложения");

    listOfError.insert(113, "Неверный идентификатор пользователя");

    listOfError.insert(150, "Неверный timestamp.");

    listOfError.insert(200, "Доступ к альбому запрещён");

    listOfError.insert(201, "Доступ к аудио запрещён");

    listOfError.insert(203, "Доступ к группе запрещён");

    listOfError.insert(300, "Альбом переполнен.");

    listOfError.insert(500, "Действие запрещено. Вы должны включить переводы голосов \r\n"
                            "в настройках приложения.");

    listOfError.insert(600, "Нет прав на выполнение данных операций \r\n"
                            "с рекламным кабинетом");

    listOfError.insert(603, "Произошла ошибка при работе с рекламным кабинетом");

    listOfError.insert(7701, "Достигнут предел общей продолжительности аудио");

    listOfError.insert(7702, "Айдиофайл слишком большой");

    listOfError.insert(7703, "Неверный hash, внутренние ошибки API");

    listOfError.insert(7704, "Задача API на распознавание не найдена");

    listOfStatus.insert("recognition_error", "ошибка распознавания речи, \r\n"
                                             "возможно, мешают фоновые шумы");

    listOfStatus.insert("transcoding_error", "ошибка перекодирования аудиозаписи. \r\n"
                                             "Используйте другой формат аудио");

    listOfStatus.insert("internal_error", "внутренние ошибки используемого API \r\n"
                                          "для распознавания речи");
}


//void NetworkRequests::onResult(QNetworkReply *reply) {

//    if (reply->error())
//    {
//        sendStatusProcess("Ошибка запроса к API");
//    }
//    else
//    {
//        switch (requestProcessingStep) {
//            case 0: {
//                QString uploadUrlFile = getUploadUrlAPI(reply);        //1 - получить адрес сервера для загрузки
//                qDebug() << "адрес сервера " << uploadUrlFile;
//                if (uploadUrlFile == "") {
//                    sendStatusProcess("ошибка при запросе Url-адреса API");
//                    requestUploadUrlAPI(filePath, modelRecognition);
//                }
//                else {
//                    ++requestProcessingStep;
//                    uploadAudioToServer(uploadUrlFile);                //2 - загрузить аудиозапись по полученному url API
//                }
//                break;
//            }
//            case 1: {
//                auto data = QJsonDocument::fromJson(reply->readAll());
//                qDebug() << "аудиоданные от сервера " << data;
//                if (listOfError.contains(data["error"]["error_code"].toInt())) {
//                    requestProcessingStep = 0;
//                    sendStatusProcess(listOfError.value(data["error"]["error_code"].toInt()));
//                }
//                else {
//                    ++requestProcessingStep;
//                    getIDforAudioProcessing(data);                     //3 - Получить id задачи на обработку аудио от API
//                }
//                break;
//            }
//            case 2: {
//                auto data = QJsonDocument::fromJson(reply->readAll());
//                qDebug() << "id задачи " << data;
//                qDebug() << listOfError.contains(data["error"]["error_code"].toInt());
//                qDebug() << data["error"]["error_code"].toInt();
//                if (listOfError.contains(data["error"]["error_code"].toInt())) {
//                    requestProcessingStep = 0;
//                    sendStatusProcess(listOfError.value(data["error"]["error_code"].toInt()));
//                }
//                else {
//                    task_id = data["response"]["task_id"].toString();
//                    ++requestProcessingStep;
//                    startAudioRecognitionProcess(task_id);             //4 - Запустить процесс распознавания аудио на сервере
//                }
//                break;
//            }
//            case 3: {
//                    auto data = QJsonDocument::fromJson(reply->readAll());
//                    qDebug() << data;
//                    if (listOfError.contains(data["error"]["error_code"].toInt())) {
//                        requestProcessingStep = 0;
//                        sendStatusProcess(listOfError.value(data["error"]["error_code"].toInt()));
//                    }
//                    if (data["response"]["status"].toString() == "recognition_error") {
//                        requestProcessingStep = 0;
//                        sendStatusProcess("ошибка распознавания речи, \r\n"
//                                          "возможно, мешают фоновые шумы");
//                    }

//                    if (data["response"]["status"].toString() == "transcoding_error") {
//                        requestProcessingStep = 0;
//                        sendStatusProcess("ошибка перекодирования аудиозаписи. \r\n"
//                                          "Используйте другой формат аудио");
//                    }

//                    if (data["response"]["status"].toString() == "internal_error") {
//                        requestProcessingStep = 0;
//                        sendStatusProcess("внутренние ошибки используемого API \r\n"
//                                          "для распознавания речи");
//                    }

//                    if (data["response"]["status"].toString() == "processing") {
//                        timerOfRetryRequests.setInterval(500);
//                        timerOfRetryRequests.start();
//                        sendStatusProcess("выполняется процесс распознавания речи, подождите");
//                    }

//                    if (data["response"]["status"].toString() == "finished") {
//                        timerOfRetryRequests.stop();
//                        requestProcessingStep = 0;
//                        sendRecognizedSpeech(data["response"]["text"].toString());
//                    }
//                    break;
//            }
//        }
//    }
//}



