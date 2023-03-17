#include "geocoderyandexapi.h"

GeocoderYandexAPI::GeocoderYandexAPI(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &GeocoderYandexAPI::onResult, Qt::QueuedConnection);
}

GeocoderYandexAPI::~GeocoderYandexAPI()
{
    delete manager;
}

void GeocoderYandexAPI::getCoordinates(QString address, QString typeRequest)
{
    if (typeRequest == "weather") {
        route = false;
        while (address[0] == "-"
               || address[0] == " "
               || address[0] == "."
               || address[0] == ",") {
            address = address.remove(0, 1);
        }
        getPointCoordinates(address);
    }

    if (typeRequest == "buildRoute") {
        route = true;
        QStringList points = address.split(" до ");
        startPointString = points.at(0);
        finishPointString = points.at(1);
        getPointCoordinates(startPointString);



        qDebug() << "Старт" << points.at(0);
        qDebug() << "Финиш" << points.at(1);
//        getPointCoordinates("Москва, Тверская семь");
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
    if (!reply->error()) {
        QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
        points << data["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"]["Point"]["pos"].toString();

        if (!route) {
            route = false;
            sendPointForWeather(points.at(0));
        }
        if (route && points.size() < 2) {
//            getPointCoordinates("Москва, Тверская 7");
            getPointCoordinates(finishPointString);
            qDebug() << points.at(0);
        }
        if (route && points.size() == 2) {
            route = false;
            qDebug() << points.at(1);
            sendPointsForRoute(points.at(0), points.at(1));
        }
    }
    else {
        qDebug() << QJsonDocument::fromJson(reply->readAll());
    }
}
