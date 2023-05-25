#include "datamapper.h"

Datamapper::Datamapper(QObject *parent) : QObject(parent)
{
}

Datamapper::~Datamapper()
{
}

bool Datamapper::readDataSQL(QSqlDatabase& db, QVector<QString>& parameters)
{
    if(db.open()) {
        QVector<QByteArray> data;
        if (!readDataSQL_query(db, data)) {
            return false;
        }
        QByteArray key, iv;
        generateKey(key, iv);
        QByteArray enc_parameter;
        for (int i = 0; i < data.size(); i++) {
            enc_parameter = data.at(i);
            parameters << decryptAES(enc_parameter, key, iv);
        }
        return true;
    }
    else {
        return false;
    }
}

bool Datamapper::readDataSQL_query(QSqlDatabase& db, QVector<QByteArray>& parameters)
{
    if (!db.tables().contains(QLatin1String("datatable"))) {
        return false;
    }
    QSqlQuery query;
    query.exec("select exists (select parameter from datatable);");
    query.next();
    if (query.value(0) !=0) {
        query.exec(QString("select * from datatable"));
        QSqlRecord rec = query.record();
        while (query.next()) {
            parameters << query.value(rec.indexOf("parameter")).toByteArray();
        }
        return true;
    }
    return false;
}

void Datamapper::generateKey(QByteArray& key, QByteArray& iv)
{
    key = QSysInfo::machineUniqueId();
    QString str_key = QString::fromLocal8Bit(key);
    QStringList lst = str_key.split("-");
    str_key = lst.join("");
    QString iv_str;
    int index = lst.size() - 1;
    for (int i = 0; i < lst.size(); i++) {
        iv_str += lst[index];
        index--;
    }
    key = str_key.toLocal8Bit();
    iv = iv_str.toLocal8Bit();
}

QString Datamapper::decryptAES(QByteArray& text, QByteArray& key, QByteArray& iv)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray hashKey = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash(iv, QCryptographicHash::Md5);
    QByteArray decodeText = encryption.decode(text, hashKey, hashIV);
    QString decodedString = QString::fromLocal8Bit(decodeText);
    decodedString.chop(1);
    return decodedString;
}

QByteArray Datamapper::encryptAES(QString inputStr, QByteArray& key, QByteArray& iv)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray hashKey = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash(iv, QCryptographicHash::Md5);
    QByteArray encodeText = encryption.encode(inputStr.toLocal8Bit(), hashKey, hashIV);
    return encodeText;
}

bool Datamapper::writeDataSQL(QSqlDatabase& db, QSqlQuery& query, int& id, QString& nameParameter, QString& valueParameter)
{
    QByteArray data1, data2, key, vector;
    generateKey(key, vector);
    data1 = encryptAES(nameParameter, key, vector);
    data2 = encryptAES(valueParameter, key, vector);
    if (!writeDataSQL_query(db, query, id, data1, data2)) {
        query.clear();
        return false;
    }
    query.clear();
    return true;
}

bool Datamapper::writeDataSQL_query(QSqlDatabase& db, QSqlQuery& query, int& id, QByteArray& nameParameter, QByteArray& valueParameter)
{
    if (!db.tables().contains(QLatin1String("datatable"))) {
        query.exec("create table datatable (id int primary key, name nvarchar(150), parameter nvarchar(700))");
    }
    query.exec(QString("select exists (select name, parameter from datatable where id = '%1');").arg(id));
    query.next();
    if (query.value(0) != 0) {
        query.prepare(QString("update datatable set name=?, parameter=? where id ='%1'").arg(id));
        query.addBindValue(nameParameter);
        query.addBindValue(valueParameter);
        if (!query.exec()) {
            query.clear();
            return false;
        }
    }
    else {
        query.prepare("insert into datatable (id, name, parameter) values (?, ?, ?)");
        query.addBindValue(id);
        query.addBindValue(nameParameter);
        query.addBindValue(valueParameter);
        if (!query.exec()) {
            query.clear();
            return false;
        }
    }
    query.clear();
    return true;
}

