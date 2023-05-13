#include "commandsexec.h"
#ifdef Q_OS_WIN
    #include "windows.h"
#endif

CommandsExec::CommandsExec(QObject *parent) : QObject(parent)
{
    execProcess = new QProcess(this);
    initListOfRequests();
    notRepeat = false;

    geoCoderYandex = new GeocoderYandexAPI(this);
    connect(this, &CommandsExec::sendAddress, geoCoderYandex, &GeocoderYandexAPI::getCoordinates);
    connect(geoCoderYandex, &GeocoderYandexAPI::sendStatusError, this, &CommandsExec::sendErrorfromCommand);
    connect(geoCoderYandex, &GeocoderYandexAPI::sendPointsForRoute, this, &CommandsExec::buildRoute);
    connect(geoCoderYandex, &GeocoderYandexAPI::sendPointForWeather, this, &CommandsExec::openYandexWeather);
    connect(this, &CommandsExec::postGeoToken, geoCoderYandex, &GeocoderYandexAPI::setYandexGeoToken);

    pathBrowser = std::getenv("PROGRAMFILES");
    pathBrowser = pathBrowser.replace("\\", "/") + "/Google/Chrome/Application/chrome.exe";
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

    for (auto item: listOfRequests.keys()) {
        rx.setPattern(QString("\\b(%1)\\b").arg(item));
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (rx.indexIn(requestStr) != -1) {
            typeOfRequest = listOfRequests.value(item).first;
            url = listOfRequests.value(item).second;
            command = item;
        }
    }

    if (typeOfRequest == "buildRoute" && !notRepeat) {
        //Построй маршрут от ... до ...
        notRepeat = true;
        requestStr = requestStr.toLower().replace(command, "");
        sendAddress(requestStr, typeOfRequest);
    }

    if (typeOfRequest == "weather" && !notRepeat) {
        //Покажи погоду в ....
        notRepeat = true;
        requestStr = requestStr.toLower().replace(command, "");
        sendAddress(requestStr, typeOfRequest);
    }

    if (typeOfRequest == "requestToDir" && !notRepeat) {
        //Найди файл на рабочем столе по имени ....
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        findAndSelectFileByQDir(requestStr);
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "requestExplorerSearch" && !notRepeat) {
        //Покажи в проводнике ... (все фотографии, все видео, все документы) с рабочего стола
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        findFileInExplorer(requestStr);
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "openFolder" && !notRepeat) {
        //Открой папку ... (общие изображения)
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        openFolderByName(requestStr);
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "openFile" && !notRepeat) {
        //открой файл ... (имя)
        notRepeat = true;
        //Открытие выделенного файла по имени, например:
        //после того как был произведён его поиск и он был найден
        openSelectFileBySearchPath(firstFindFile);
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "minimizeActiveWindow" && !notRepeat) {
        //Окно свери...
        notRepeat = true;
        minimizeActiveWindow();
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "closeWindow" && !notRepeat) {
        //Закрой окно... (активное)
        notRepeat = true;
        closeActiveWindow();
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "openResource" && !notRepeat) {
        //открой гугл, открой яндекс, открой гугл диск, открой гугл переводчик, открой ютуб и тд
        notRepeat = true;
        openInternetResource(url);
        sendStatusProcess("ожидание голосовой команды");
    }

    if (typeOfRequest == "requestToSearch" && !notRepeat) {
        //Найди в гугл, найди в яндексе, найди на ютуб...
        requestStr = requestStr.toLower().replace(command,"");
        notRepeat = true;
        searchTheInternet(requestStr, url);
        sendStatusProcess("ожидание голосовой команды");
    }

    if (command == "") {
        sendErrorToMainWindow(tr("Ошибка! Запрошенная команда не найдена!"));
    }
}

void CommandsExec::initListOfRequests()
{
    listOfRequests.insert("построй, маршрут от", {"buildRoute", ""});
    listOfRequests.insert("построй. маршрут от", {"buildRoute", ""});
    listOfRequests.insert("построй маршрут от ", {"buildRoute", ""});
    listOfRequests.insert("маршрут от ", {"buildRoute", ""});
    listOfRequests.insert("строй, маршрут от ", {"buildRoute", ""});

    listOfRequests.insert("покажи погоду в городе", {"weather", ""});
    listOfRequests.insert("покажи погоду в", {"weather", ""});
    listOfRequests.insert("покажи погоду", {"weather", ""});
    listOfRequests.insert("скажи погоду", {"weather", ""});
    listOfRequests.insert("погода в", {"weather", ""});
    listOfRequests.insert("скажи погоду в", {"weather", ""});
    listOfRequests.insert("скажи погоду в городе", {"weather", ""});

    listOfRequests.insert("открой поиск гугл.", {"openResource", "https://www.google.com/"});
    listOfRequests.insert("Открой гугл-поиск.", {"openResource", "https://www.google.com/"});
    listOfRequests.insert("Открой гугл", {"openResource", "https://www.google.com/"});
    listOfRequests.insert("открой поиск google", {"openResource", "https://www.google.com/"});
    listOfRequests.insert("открой google поиск", {"openResource", "https://www.google.com/"});

    listOfRequests.insert("открой гугл диск", {"openResource", "https://drive.google.com/"});
    listOfRequests.insert("открой google диск", {"openResource", "https://drive.google.com/"});
    listOfRequests.insert("открой мой гугл диск", {"openResource", "https://drive.google.com/"});
    listOfRequests.insert("открой мой гугл-диск.", {"openResource", "https://drive.google.com/"});
    listOfRequests.insert("крой мой гугл-диск.", {"openResource", "https://drive.google.com/"});
    listOfRequests.insert("открой мой google диск", {"openResource", "https://drive.google.com/"});

    listOfRequests.insert("открой гугл. переводчик", {"openResource", "https://translate.google.ru/"});
    listOfRequests.insert("открой переводчик гугл", {"openResource", "https://translate.google.ru/"});
    listOfRequests.insert("крой гугл. переводчик.", {"openResource", "https://translate.google.ru/"});
    listOfRequests.insert("открой переводчик, гугл.", {"openResource", "https://translate.google.ru/"});
    listOfRequests.insert("открой google переводчик", {"openResource", "https://translate.google.ru/"});
    listOfRequests.insert("открой переводчик google", {"openResource", "https://translate.google.ru/"});

    listOfRequests.insert("открой яндекс", {"openResource", "https://yandex.ru/"});
    listOfRequests.insert("открой ютуб", {"openResource", "https://www.youtube.com/"});
    listOfRequests.insert("открой youtube", {"openResource", "https://www.youtube.com/"});

    listOfRequests.insert("найди в гугл", {"requestToSearch", "https://www.google.com/search?q="});
    listOfRequests.insert("найди в google", {"requestToSearch", "https://www.google.com/search?q="}); //яндекс
    listOfRequests.insert("найти в google", {"requestToSearch", "https://www.google.com/search?q="});
    listOfRequests.insert("поиск в гугле", {"requestToSearch", "https://www.google.com/search?q="});
    listOfRequests.insert("поиск в гугл", {"requestToSearch", "https://www.google.com/search?q="});
    listOfRequests.insert("поиск гугл", {"requestToSearch", "https://www.google.com/search?q="});
    listOfRequests.insert("найди в гугле", {"requestToSearch", "https://www.google.com/search?q="});
    listOfRequests.insert("загугли", {"requestToSearch", "https://www.google.com/search?q="});

    listOfRequests.insert("найди в яндексе", {"requestToSearch", "https://yandex.ru/search/?text="});
    listOfRequests.insert("найти в яндексе что нибудь", {"requestToSearch", "https://yandex.ru/search/?text="});
    listOfRequests.insert("найди в яндекс", {"requestToSearch", "https://yandex.ru/search/?text="});

    listOfRequests.insert("найди в ютуб", {"requestToSearch", "https://www.youtube.com/results?search_query="});
    listOfRequests.insert("найди в ютуб-видео", {"requestToSearch", "https://www.youtube.com/results?search_query="});
    listOfRequests.insert("найди в youtube", {"requestToSearch", "https://www.youtube.com/results?search_query="});
    listOfRequests.insert("найди в youtube видео", {"requestToSearch", "https://www.youtube.com/results?search_query="});

    listOfRequests.insert("найди файл по имени на рабочем столе", {"requestToDir", ""});
    listOfRequests.insert("найди файл на рабочем столе по имени", {"requestToDir", ""});
    listOfRequests.insert("найди на рабочем столе файл по имени", {"requestToDir", ""});
    listOfRequests.insert("найди на рабочем столе файл", {"requestToDir", ""});

    listOfRequests.insert("покажи в проводнике", {"requestExplorerSearch", ""});

    listOfRequests.insert("открой папку", {"openFolder", ""});
    listOfRequests.insert("крой папку", {"openFolder", ""});

    listOfRequests.insert("файл. открой", {"openFile", ""});
    listOfRequests.insert("открой файл", {"openFile", ""});

    listOfRequests.insert("сверни окно", {"minimizeActiveWindow", ""});
    listOfRequests.insert("окно сверни", {"minimizeActiveWindow", ""});

    listOfRequests.insert("закрой окно", {"closeWindow", ""});
    listOfRequests.insert("закрой файл", {"closeWindow", ""});
}

void CommandsExec::resendGeoToken(QString token)
{
    postGeoToken(token);
}

void CommandsExec::requestCorrection(QString &query)
{
    while (query[0] == "-"
           || query[0] == " "
           || query[0] == "."
           || query[0] == ",") {
        query = query.remove(0, 1);
    }
    int length = query.size();
    while (query[length-1] == "-"
           || query[length-1] == " "
           || query[length-1] == "."
           || query[length-1] == ",") {
        query = query.remove(length-1, 1);
    }
}

void CommandsExec::searchTheInternet(QString& requestStr, QString& url)
{
    QString resultUrl = "";
    requestCorrection(requestStr);
    resultUrl = url + requestStr;
    resultUrl.replace(" ","+");
    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << resultUrl;
    notRepeat = false;
    execProcess->start(pathBrowser, arguments);
}

void CommandsExec::openInternetResource(QString& url)
{
    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << url;
    notRepeat = false;
    execProcess->start(pathBrowser, arguments);
}

void CommandsExec::sendErrorfromCommand(QString errorMessage)
{
    notRepeat = false;
    sendErrorToMainWindow(errorMessage);
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
    execProcess->start(pathBrowser, arguments);
    sendStatusProcess("ожидание голосовой команды");
}

void CommandsExec::openYandexWeather(QString location)
{
    QStringList point = location.split(" ");
    QString latitude = point.at(1);
    QString longitude = point.at(0);
    QString url("https://yandex.ru/pogoda/?");
    url += "lat=";
    url += latitude;
    url+= "&lon=";
    url += longitude;
    arguments.clear();
    arguments << "--chrome-frame" << "-kiosk" << url;
    notRepeat = false;
    execProcess->start(pathBrowser, arguments);
    sendStatusProcess("ожидание голосовой команды");
}

void CommandsExec::openFolderByName(QString folderName)
{
    requestCorrection(folderName);
    QString pathImages = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    pathImages = pathImages.section("/", 0, 1) + "/Public/Pictures";
    if (folderName == "общее изображение"
            || folderName == "общие изображения") {
        desktopService.openUrl(QUrl::fromLocalFile(pathImages));
    }
    notRepeat = false;
}

void CommandsExec::openSelectFileBySearchPath(QString path)
{
    requestCorrection(path);
    if (!path.isEmpty()) {
        desktopService.openUrl(QUrl::fromLocalFile(path));
    }
    notRepeat = false;
}

void CommandsExec::minimizeActiveWindow()
{
    #ifdef Q_OS_WIN
        HWND hWnd = GetForegroundWindow();
        notRepeat = false;
        //Сворачиваем активное окно /окно с которым в данный момент работает пользователь
        ShowWindow(hWnd, SW_SHOWMINIMIZED);
    #else
        sendErrorToMainWindow("На текущий момент, данная голосовая команда \r\n"
                              "доступна только на ОС Windows");
    #endif
}

void CommandsExec::closeActiveWindow()
{
    #ifdef Q_OS_WIN
        HWND hWnd = GetForegroundWindow();
        notRepeat = false;
        //Закрываем активное окно /окно с которым в данный момент работает пользователь
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    #else
        sendErrorToMainWindow("На текущий момент, данная голосовая команда \r\n"
                          "доступна только на ОС Windows");
    #endif
}

void CommandsExec::findFileInExplorer(QString command)
{
    //Экранироание команды cmd достигается за счёт \"команда\"
    //Экранирование обратного слэша достигается ещё одним обратным слэшем
    bool foundType = false;
    bool foundDir = false;
    requestCorrection(command);
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
    foreach (QString subdir, listDir) {
        if (subdir == "." || subdir == "..") {
            continue;
        }
        searhPathFile(QDir(dir.absoluteFilePath(subdir)), files, fileName);
    }
}

void CommandsExec::findAndSelectFileByQDir(QString fileName)
{
    requestCorrection(fileName);
    //Чтобы поиск осуществлялся только по названию файла, без указания расширения, нужно поставить * после имени файла
    sendStatusProcess("запущен поиска файла " + fileName);
    fileName += "*";
    listFiles.clear();
    searhPathFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), listFiles, fileName);
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
