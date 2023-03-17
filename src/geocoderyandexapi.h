#ifndef GEOCODERYANDEXAPI_H
#define GEOCODERYANDEXAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

class GeocoderYandexAPI : public QObject
{
    Q_OBJECT
public:
    explicit GeocoderYandexAPI(QObject *parent = nullptr);
    ~GeocoderYandexAPI();

private:

    QNetworkAccessManager* manager;

    QString startPointString;
    QString finishPointString;
    QStringList points;
    bool route = false;
    void getPointCoordinates(QString);

    //Key для геокодера яндекс-карт
    QString geoCoderKey;

    /*
    Пример простого запроса координат
    https://geocode-maps.yandex.ru/1.x/?apikey=ваш API-ключ&format=json&geocode=Москва,+Тверская+улица,+дом+7
    */

signals:

    void sendPointForWeather(QString);
    void sendPointsForRoute(QString, QString);

public slots:

    void getCoordinates(QString, QString);

private slots:

    void onResult(QNetworkReply* reply);
};

#endif // GEOCODERYANDEXAPI_H
