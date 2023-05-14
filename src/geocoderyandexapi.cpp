#include "geocoderyandexapi.h"

GeocoderYandexAPI::GeocoderYandexAPI(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &GeocoderYandexAPI::onResult, Qt::QueuedConnection);
    initListOfErrors();
}

GeocoderYandexAPI::~GeocoderYandexAPI()
{
    delete manager;
}

void GeocoderYandexAPI::getCoordinates(QString address, QString typeRequest)
{
    if (typeRequest == "weather") {
        route = false;
        weatherRequestCorrection(address);
        getPointCoordinates(address);
    }

    if (typeRequest == "buildRoute") {
        route = true;
        QStringList points = address.split(" до ");
        startPointString = points.at(0);
        finishPointString = points.at(1);
        startCoordinates = "";
        finishCoordinates = "";
        getPointCoordinates(startPointString);
    }
}

void GeocoderYandexAPI::weatherRequestCorrection(QString &query)
{
    while (query[0] == "-"
           || query[0] == " "
           || query[0] == "."
           || query[0] == ",") {
        query = query.remove(0, 1);
    }
    int length = query.size();
    while (query[length-1] == "-"
           || query[length-1] == " "
           || query[length-1] == "."
           || query[length-1] == ",") {
        query = query.remove(length-1, 1);
    }
}

void GeocoderYandexAPI::getPointCoordinates(QString address)
{
    address = address.replace(" ", "+");
    QString apiUrl("https://geocode-maps.yandex.ru/1.x/?");
    QNetworkRequest request;
    QUrl url(apiUrl);
    QUrlQuery query(url.query());
    query.addQueryItem("apikey", geoCoderKey);
    query.addQueryItem("format", "json");
    query.addQueryItem("geocode", address);
    url.setQuery(query);
    request.setUrl(url);
    manager->get(request);
}


void GeocoderYandexAPI::onResult(QNetworkReply *reply)
{    
    bool error = false;
    QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
    if (reply->error()) {
        error = true;
        if (listOfErrors.contains(data["error"].toString())) {
            emit sendStatusError(listOfErrors.value(data["error"].toString()));
        }
        else {
            emit sendStatusError(data["message"].toString());
        }
    }

    if (!error) {
        pointPosition = data["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"]["Point"]["pos"].toString();

        if (!route) {
            emit sendPointForWeather(pointPosition);
        }

        if (route) {
            if (startCoordinates != "" && finishCoordinates =="") {
                finishCoordinates = pointPosition;
                if (finishCoordinates !="") {
                    route = false;
                    emit sendPointsForRoute(startCoordinates, finishCoordinates);
                }
                else {
                    getPointCoordinates(finishPointString);
                }
            }
            if (startCoordinates == "" && finishCoordinates =="") {
                startCoordinates = pointPosition;
                if (startCoordinates !="") {
                    getPointCoordinates(finishPointString);
                }
                else {
                    getPointCoordinates(startPointString);
                }
            }
        }
    }
}

void GeocoderYandexAPI::setYandexGeoToken(QString token)
{
    geoCoderKey = token;
}

void GeocoderYandexAPI::initListOfErrors()
{
    listOfErrors.insert("Bad Request", "Синтаксическая ошибка в запросе к API геокодера");
    listOfErrors.insert("Forbidden", "Ошибка авторизации в API геокодера");
}
