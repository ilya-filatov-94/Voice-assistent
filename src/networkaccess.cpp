#include "networkaccess.h"

NetworkAccess::NetworkAccess(QObject *parent) : QObject(parent)
{
    connection_dns = false;
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));
}

NetworkAccess::~NetworkAccess()
{
    delete manager;
}

void NetworkAccess::checkNetworkConnection()
{
    httpRequestNCSI();
}

void NetworkAccess::httpRequestNCSI()
{
    QUrl url("http://www.msftncsi.com/ncsi.txt");
    QNetworkRequest request;
    request.setUrl(url);
    manager->get(request);
}

void NetworkAccess::onResult(QNetworkReply* reply)
{
    if (reply->error()) {
        emit sendStatusNetworkConnection(false);
    }
    else {
        QString str = reply->readAll();
        if (str == "Microsoft NCSI") {
            requestDNS();
        }
        else {
            emit sendStatusNetworkConnection(false);
        }
    }
}

void NetworkAccess::requestDNS()
{
    QHostInfo::lookupHost("dns.msftncsi.com",this, SLOT(lookedUp(QHostInfo)));
}

void NetworkAccess::lookedUp(const QHostInfo &search_host)
{
   if (search_host.error() != QHostInfo::NoError) {
       connection_dns=false;
   }
   foreach (const QHostAddress &address, search_host.addresses()) {
       if (ip_list.contains(address.toString()))
       {
           connection_dns=true;
           break;
       }
   }
   if (!connection_dns) {
        emit sendStatusNetworkConnection(false);
   }
}
