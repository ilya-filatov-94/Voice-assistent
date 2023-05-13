#ifndef DATAMAPPER_H
#define DATAMAPPER_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>
#include <qaesencryption.h>
#include <qrsaencryption.h>

class Datamapper : public QObject
{
    Q_OBJECT
public:
    explicit Datamapper(QObject *parent = nullptr);
    ~Datamapper();
    bool readDataSQL(QSqlDatabase&, QVector<QString>&);

private:

    bool readDataSQL_query(QSqlDatabase&, QVector<QByteArray>&);
    void generateKey(QByteArray&, QByteArray&);
    QString decryptAES(QByteArray&, QByteArray&, QByteArray&);

};

#endif // DATAMAPPER_H
