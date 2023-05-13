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

    bool route = false;
    QString pointPosition;
    QString startPointString, finishPointString;
    QString startCoordinates, finishCoordinates;
    void getPointCoordinates(QString);
    QString geoCoderKey;

    void weatherRequestCorrection(QString&);
    void initListOfErrors();
    QMap<QString, QString> listOfErrors;

signals:

    void sendStatusError(QString);
    void sendPointForWeather(QString);
    void sendPointsForRoute(QString, QString);

public slots:

    void getCoordinates(QString, QString);
    void setYandexGeoToken(QString);

private slots:

    void onResult(QNetworkReply* reply);
};

#endif // GEOCODERYANDEXAPI_H
