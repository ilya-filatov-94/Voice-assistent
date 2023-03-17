#include "commandsexec.h"
#include <QDebug>

CommandsExec::CommandsExec(QObject *parent) : QObject(parent)
{
    execProcess = new QProcess(this);
    initlistUrls();
    notRepeat = false;

    geoCoderYandex = new GeocoderYandexAPI(this);
    connect(this, &CommandsExec::sendAddress, geoCoderYandex, &GeocoderYandexAPI::getCoordinates);
    connect(geoCoderYandex, &GeocoderYandexAPI::sendPointsForRoute, this, &CommandsExec::buildRoute);
    connect(geoCoderYandex, &GeocoderYandexAPI::sendPointForWeather, this, &CommandsExec::openYandexWeather);
}

CommandsExec::~CommandsExec()
{
    delete execProcess;
    delete geoCoderYandex;
}

void CommandsExec::choose_action(QString requestStr)
{
    QString typeOfRequest;
    QRegExp rx;
    QString url;
    QString command;

    for (auto item: listUrls.keys()) {
        rx.setPattern(QString("\\b(%1)\\b").arg(item));
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (rx.indexIn(requestStr) != -1) {
            typeOfRequest = listUrls.value(item).first;
            url = listUrls.value(item).second;
            command = item;
        }
    }

    qDebug() << "Команда" << command;

    if (typeOfRequest == "requestToSearch" && !notRepeat) {
        notRepeat = true;
        searchTheInternet(command, requestStr, url);
    }

    if (typeOfRequest == "openResource" && !notRepeat) {
        notRepeat = true;
        openInternetResource(url);
    }

    if (typeOfRequest == "buildRoute" && !notRepeat) {
        notRepeat = true;
        requestStr = requestStr.remove(0, 20);
        sendAddress(requestStr, typeOfRequest);
    }

    if (typeOfRequest == "weather" && !notRepeat) {
        notRepeat = true;
        requestStr = requestStr.toLower().replace(command, "");
        sendAddress(requestStr, typeOfRequest);
    }

    if (typeOfRequest == "openFolder" && !notRepeat) {
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        openFolderByName(requestStr);
    }

    if (typeOfRequest == "openFile" && !notRepeat) {
        notRepeat = true;
        openFileBySearchPath(firstFindFile);
    }

    if (typeOfRequest == "minimizeActiveWindow" && !notRepeat) {
        notRepeat = true;
        minimizeActiveWindow();
    }

    if (typeOfRequest == "closeWindow" && !notRepeat) {
        notRepeat = true;
        closeActiveWindow();
    }

    if (typeOfRequest == "requestExplorerSearch" && !notRepeat) {
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        findFileInExplorer(requestStr);
    }

    if (typeOfRequest == "requestToDir" && !notRepeat) {
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        findAndSelectFileByQDir(requestStr);
    }
}

void CommandsExec::initlistUrls()
{
    listUrls.insert("найди в гугл", {"requestToSearch", "https://www.google.com/search?q="});
    listUrls.insert("поиск в гугле", {"requestToSearch", "https://www.google.com/search?q="});
    listUrls.insert("поиск в гугл", {"requestToSearch", "https://www.google.com/search?q="});
    listUrls.insert("поиск гугл", {"requestToSearch", "https://www.google.com/search?q="});
    listUrls.insert("найди в гугле", {"requestToSearch", "https://www.google.com/search?q="});
    listUrls.insert("загугли", {"requestToSearch", "https://www.google.com/search?q="});
    listUrls.insert("найди в яндексе", {"requestToSearch", "https://yandex.ru/search/?text="});
    listUrls.insert("найди в яндекс", {"requestToSearch", "https://yandex.ru/search/?text="});
    listUrls.insert("найди в ютуб", {"requestToSearch", "https://www.youtube.com/results?search_query="});
    listUrls.insert("найди в ютуб-видео", {"requestToSearch", "https://www.youtube.com/results?search_query="});
    listUrls.insert("найди файл по имени на рабочем столе", {"requestToDir", ""});
    listUrls.insert("найди файл на рабочем столе по имени", {"requestToDir", ""});
    listUrls.insert("покажи в проводнике", {"requestExplorerSearch", ""});
    listUrls.insert("открой папку", {"openFolder", ""});
    listUrls.insert("файл. открой", {"openFile", ""});
    listUrls.insert("сверни", {"minimizeActiveWindow", ""});
    listUrls.insert("закрой", {"closeWindow", ""});

    listUrls.insert("открой поиск гугл", {"openResource", "https://www.google.com/"});
    listUrls.insert("открой гугл поиск", {"openResource", "https://www.google.com/"});
    listUrls.insert("открой гугл диск", {"openResource", "https://drive.google.com/"});
    listUrls.insert("открой мой гугл диск", {"openResource", "https://drive.google.com/"});
    listUrls.insert("открой гугл. переводчик", {"openResource", "https://translate.google.ru/"});
    listUrls.insert("открой переводчик гугл", {"openResource", "https://translate.google.ru/"});
    listUrls.insert("открой яндекс", {"openResource", "https://yandex.ru/"});
    listUrls.insert("открой ютуб", {"openResource", "https://www.youtube.com/"});
    listUrls.insert("построй, маршрут от", {"buildRoute", ""});
    listUrls.insert("построй. маршрут от", {"buildRoute", ""});
    listUrls.insert("покажи погоду", {"weather", ""});

}

void CommandsExec::searchTheInternet(QString& command, QString& requestStr, QString& url)
{
    QString resultUrl = "";
    QString browser = "C:/Program Files/Google/Chrome/Application/chrome.exe";
    requestStr = requestStr.toLower().replace(command,"");
    while (requestStr[0] == "-"
           || requestStr[0] == " "
           || requestStr[0] == "."
           || requestStr[0] == ",") {
        requestStr = requestStr.remove(0, 1);
    }
    resultUrl = url + requestStr;
    resultUrl.replace(" ","+");
    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << resultUrl;
    notRepeat = false;
    execProcess->start(browser, arguments);
}

void CommandsExec::openInternetResource(QString& url)
{
    QString browser = "C:/Program Files/Google/Chrome/Application/chrome.exe";
    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << url;
    notRepeat = false;
    execProcess->start(browser, arguments);
}

void CommandsExec::buildRoute(QString start, QString finish)
{
    QStringList startPoint = start.split(" ");
    QStringList finishPoint = finish.split(" ");
    QString latitude1, longitude1, latitude2, longitude2;
    latitude1 = startPoint.at(1);
    longitude1 = startPoint.at(0);
    latitude2 = finishPoint.at(1);
    longitude2 = finishPoint.at(0);

    QString browser = "C:/Program Files/Google/Chrome/Application/chrome.exe";
    QString urlRoute("https://yandex.ru/maps/?rtext=");
    urlRoute += latitude1;
    urlRoute += ",";
    urlRoute += longitude1;
    urlRoute += "~";
    urlRoute += latitude2;
    urlRoute += ",";
    urlRoute += longitude2;
    //rtt - Тип маршрута: auto (автомобильный маршрут), mt (общественный транспорт), pd (пешеходный маршрут), bc (велосипедный маршрут).
    urlRoute += "&rtt=mt";
    //Включить показ пробок l=trf
    urlRoute += "&l=trf";

    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << urlRoute;
    notRepeat = false;
    execProcess->start(browser, arguments);
}

void CommandsExec::openYandexWeather(QString location)
{
    //    https://yandex.ru/pogoda/?lat=55.81579208&lon=37.38003159&via=srp
    QStringList point = location.split(" ");
    QString latitude = point.at(1);
    QString longitude = point.at(0);
    QString url("https://yandex.ru/pogoda/?");
    url += "lat=";
    url += latitude;
    url+= "&lon=";
    url += longitude;

    QString browser = "C:/Program Files/Google/Chrome/Application/chrome.exe";
    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << url;
    notRepeat = false;
    execProcess->start(browser, arguments);
}

void CommandsExec::openFolderByName(QString folderName)
{
    while (folderName[0] == "-"
           || folderName[0] == " "
           || folderName[0] == "."
           || folderName[0] == ",") {
        folderName = folderName.remove(0, 1);
    }
    QString path;
    if (folderName == "общее изображение."
            || folderName == "общие изображения.") {
        path = "C:/Users/Public/Pictures/";
        desktopService.openUrl(QUrl::fromLocalFile(path));
    }
    qDebug() << path;
    notRepeat = false;
}

void CommandsExec::openFileBySearchPath(QString path)
{
    while (path[0] == "-"
           || path[0] == " "
           || path[0] == "."
           || path[0] == ",") {
        path = path.remove(0, 1);
    }
    if (!path.isEmpty()) {
        desktopService.openUrl(QUrl::fromLocalFile(path));
    }
    notRepeat = false;
}

void CommandsExec::minimizeActiveWindow()
{
    HWND hWnd = GetForegroundWindow();
    notRepeat = false;
    //Сворачиваем активное окно /окно с которым в данный момент работает пользователь
    ShowWindow(hWnd, SW_SHOWMINIMIZED);
}

void CommandsExec::closeActiveWindow()
{
    HWND hWnd = GetForegroundWindow();
    notRepeat = false;
    //Закрываем активное окно /окно с которым в данный момент работает пользователь
    PostMessage(hWnd, WM_CLOSE, 0, 0);
}

void CommandsExec::findFileInExplorer(QString command)
{
    //Экранироание команды cmd достигается за счёт \"команда\"
    //Экранирование обратного слыэша достигается ещё одним обратным слэшем
    bool foundType = false;
    bool foundDir = false;
    while (command[0] == "-"
           || command[0] == " "
           || command[0] == "."
           || command[0] == ",") {
        command = command.remove(0, 1);
    }
    QString currentType, currentDir, request;

    QRegExp rx;
    rx.setPattern("\\b(все фотографии)\\b");
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (rx.indexIn(command) != -1) {
        currentType = "*.jpg OR *.png";
        foundType = true;
        command = command.toLower().replace("все фотографии ","");
    }

    if (!foundType) {
        rx.setPattern("\\b(все документы)\\b");
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (rx.indexIn(command) != -1) {
            currentType = "*.docx OR *.doc OR *.pdf";
            foundType = true;
            command = command.toLower().replace("все документы ","");
        }
    }

    if (!foundType) {
        rx.setPattern("\\b(все видео)\\b");
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (rx.indexIn(command) != -1) {
            currentType = "*.mp4 OR *.MOV";
            foundType = true;
            command = command.toLower().replace("все видео ","");
        }
    }

    if (!foundDir) {
        rx.setPattern("\\b(с рабочего стола)\\b");
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (rx.indexIn(command) != -1) {
            currentDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            currentDir.replace("/", "\\");
            currentDir += "&";
//            currentDir = "C:\\Users\\<user>\\Desktop&";
            foundDir = true;
            request = "@start explorer.exe \"search-ms:query=";
            request += currentType;
            request += "&crumb=location:";
            request += currentDir;
            request += "\"";
        }
    }

    QByteArray bytes = request.toLocal8Bit();
    const char *c_str = bytes.data();
    std::system(c_str);
    notRepeat = false;
    //Выполнить команду поиска файла в проводнике windows
    //    std::system("@start explorer.exe \"search-ms:query=a*.pdf&crumb=location:c:\&\"");
    //    std::system("@start explorer.exe \"search-ms:query=IMG_9356&crumb=location:C:\\Users&\"");
    //Поиск в общих изображениях
    //    std::system("@start explorer.exe \"search-ms:query=*.jpg OR *.png&crumb=location:C:\\Users\\Public\\Pictures&\"");
    //    std::system("@start explorer.exe \"search-ms:query=*.jpg OR *.png&crumb=location:C:\\Users\\506\\Desktop\\from_Desktop_11_03_22&\"");

}

void CommandsExec::searhPathFile(QDir dir, QStringList& files, QString& fileName)
{
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QStringList listFiles = dir.entryList(QStringList() << fileName, QDir::Files);
    if (!listFiles.isEmpty()) {
        tempList.clear();
        tempList << dir.absolutePath() << listFiles;
        files << tempList;
    }

    QStringList listDir = dir.entryList(QDir::Dirs);
    foreach (QString subdir, listDir)
    {
        if (subdir == "." || subdir == "..")
        {
            continue;
        }
        searhPathFile(QDir(dir.absoluteFilePath(subdir)), files, fileName);      //Рекурсивный вызов метода
    }
}

void CommandsExec::findAndSelectFileByQDir(QString fileName)
{
    //Поиск файла по заданному пути только по имени
    while (fileName[0] == "-"
           || fileName[0] == " "
           || fileName[0] == "."
           || fileName[0] == ",") {
        fileName = fileName.remove(0, 1);
    }
    if (fileName[fileName.size()-1] == "-"
            || fileName[fileName.size()-1] == " "
            || fileName[fileName.size()-1] == ".") {
        fileName = fileName.remove(fileName.size()-1, 1);
    }
    //Чтобы поиск осуществлялся только по названию файла, без указания расширения, нужно поставить * после имени файла
    fileName += "*";
    qDebug() << "Запуск поиска файла" << fileName;
    listFiles.clear();
    searhPathFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), listFiles, fileName);
//    searhPathFile(QDir("C:/Users/<user>/Desktop"), listFiles, fileName);  //открытие диска и поиск файла
    QApplication::processEvents(QEventLoop::AllEvents);

    tempList.clear();
    QString tempPath;
    foreach (QString item, listFiles) {
        QString tempFile;
        if (item.contains("C:/", Qt::CaseInsensitive) == true) {
            tempPath = item;
        }
        if (item.contains("C:/", Qt::CaseInsensitive) == false) {
            tempFile = tempPath + "/" + item;
            tempList << tempFile;
        }
    }
    qDebug() << tempList;   //список абсолютных путей найденных файлов
    if (!tempList.isEmpty()) {
        firstFindFile = tempList.at(0);
    }

    //Открываем проводник и выделяем 1-ый найденный файл по заданному пути
    arguments.clear();
    arguments << QLatin1String("/select,");
    arguments << QDir::toNativeSeparators(tempList.at(0));
    execProcess->start("explorer", arguments);
    notRepeat = false;
}
