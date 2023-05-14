#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QHostInfo>
#include <QList>

class NetworkAccess : public QObject
{
    Q_OBJECT
public:
    explicit NetworkAccess(QObject *parent = nullptr);
    ~NetworkAccess();
    void checkNetworkConnection();

private:
    QNetworkAccessManager* manager;
    QList<QString> ip_list={"131.107.255.255"};
    bool connection_dns;

    void httpRequestNCSI();
    void requestDNS();

signals:
    void sendStatusNetworkConnection(bool);

private slots:

    void onResult(QNetworkReply* reply);
    void lookedUp(const QHostInfo &search_host);

};

#endif // NETWORKACCESS_H
