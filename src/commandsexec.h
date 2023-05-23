#ifndef COMMANDSEXEC_H
#define COMMANDSEXEC_H

#include <QApplication>
#include <QObject>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QCursor>
#include <QStringList>
#include <QDirIterator>
#include <QMap>

#include "geocoderyandexapi.h"

#include <QDebug>

class CommandsExec : public QObject
{
    Q_OBJECT
public:
    explicit CommandsExec(QObject *parent = nullptr);
    ~CommandsExec();

signals:

    void sendAddress(QString, QString);
    void sendErrorToMainWindow(QString);
    void sendStatusProcess(QString);
    void postGeoToken(QString);

private:
    bool notRepeat;
    QProcess* execProcess;
    QDesktopServices desktopService;
    QStringList arguments;
    GeocoderYandexAPI* geoCoderYandex;
    QMap<QString, QPair<QString, QString>> listOfRequests;
    QStringList listFiles;
    QStringList tempList;
    QString firstFindFile;
    QString pathBrowser;

    void initListOfRequests();
    void requestCorrection(QString&);
    void searchTheInternet(QString&, QString&);
    void openInternetResource(QString&);
    void closeActiveTabBrowser();
    void sendErrorfromCommand(QString);
    void buildRoute(QString, QString);
    void openYandexWeather(QString);
    void openFolderByName(QString);
    void openSelectFileBySearchPath(QString);
    void minimizeActiveWindow();
    void closeActiveWindow();
    void findFileInExplorer(QString);
    void searhPathFile(QDir, QStringList&, QString&);
    void findAndSelectFileByQDir(QString);


public slots:
    void resendGeoToken(QString);
    void choose_action(QString);

private slots:
    void resultProcess(int, QProcess::ExitStatus);

};

#endif // COMMANDSEXEC_H
