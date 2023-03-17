#ifndef COMMANDSEXEC_H
#define COMMANDSEXEC_H

#include "windows.h"
#include "geocoderyandexapi.h"

#include <QApplication>
#include <QObject>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QCursor>
#include <QStringList>
#include <QDirIterator>
#include <QMap>

class CommandsExec : public QObject
{
    Q_OBJECT
public:
    explicit CommandsExec(QObject *parent = nullptr);
    ~CommandsExec();

signals:

    void sendAddress(QString, QString);

private:
    bool notRepeat;
    QProcess* execProcess;
    QDesktopServices desktopService;
    QStringList arguments;
    GeocoderYandexAPI* geoCoderYandex;
    QMap<QString, QPair<QString, QString>> listUrls;
    QStringList listFiles;
    QStringList tempList;
    QString firstFindFile;

    void initlistUrls();
    void searchTheInternet(QString&, QString&, QString&);
    void openInternetResource(QString&);
    void buildRoute(QString, QString);
    void openYandexWeather(QString);
    void openFolderByName(QString);
    void openFileBySearchPath(QString);
    void minimizeActiveWindow();
    void closeActiveWindow();
    void findFileInExplorer(QString);
    void searhPathFile(QDir, QStringList&, QString&);
    void findAndSelectFileByQDir(QString);

public slots:
    void choose_action(QString);

};

#endif // COMMANDSEXEC_H
