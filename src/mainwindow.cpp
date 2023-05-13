#include "mainwindow.h"
//#include <stdio.h>
//#include <string.h>

//#include <QApplication>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    widgetWindow = new QWidget(this);
    widgetWindow->setObjectName("widgetWindow");
    createUserInterface();
    setCentralWidget(widgetWindow);

    dataMapper = new Datamapper(this);
    recorder = new AudioRecorder(this);
    statusRecord = false;
    changingServiceSlider(true);
    changingModeSlider(true);
    connect(this, &MainWindow::setServiceRecognition, recorder, &AudioRecorder::changeServiceRecognition);
    connect(this, &MainWindow::commandRecord, recorder, &AudioRecorder::toggleRecord);
    connect(this, &MainWindow::sendPathToFFMPEG, recorder, &AudioRecorder::setPathToFFMPEG);
    connect(recorder, &AudioRecorder::sendStatusProcess, this, &MainWindow::updateStatusProcess);
    connect(recorder, &AudioRecorder::sendPathToAudioFile, this, &MainWindow::getPathToAudioFile);

    VkSpeechRec = new VkASRspeechRecognition(this);
    connect(this, &MainWindow::sendVkSpeechToken, VkSpeechRec, &VkASRspeechRecognition::setVkSpeechToken);
    connect(this, &MainWindow::postRequestToVkAPI, VkSpeechRec, &VkASRspeechRecognition::requestUploadUrlAPI);
    connect(VkSpeechRec, &VkASRspeechRecognition::sendStatusProcess, this, &MainWindow::updateStatusProcess);
    connect(VkSpeechRec, &VkASRspeechRecognition::sendStatusError, this, &MainWindow::speechRecognitionError);
    connect(VkSpeechRec, &VkASRspeechRecognition::sendRecognizedSpeech, this, &MainWindow::getRecognizedSpeech);

    yaSpeechRec = new YandexSpeechRecognition(this);
    connect(this, &MainWindow::sendYandexSpeechToken, yaSpeechRec, &YandexSpeechRecognition::setYandexSpeechToken);
    connect(this, &MainWindow::postRequestToYandexAPI, yaSpeechRec, &YandexSpeechRecognition::postAudioToAPI);
    connect(yaSpeechRec, &YandexSpeechRecognition::sendStatusError, this, &MainWindow::speechRecognitionError);
    connect(yaSpeechRec, &YandexSpeechRecognition::sendRecognizedSpeech, this, &MainWindow::getRecognizedSpeech);

    assistant = new CommandsExec(this);
    connect(this, &MainWindow::sendYandexGeoToken, assistant, &CommandsExec::resendGeoToken);
    connect(this, &MainWindow::requestCommand, assistant, &CommandsExec::choose_action);
    connect(assistant, &CommandsExec::sendErrorToMainWindow, this, &MainWindow::speechRecognitionError);
    connect(assistant, &CommandsExec::sendStatusProcess, this, &MainWindow::updateStatusProcess);

    setWindowTitle(tr("Голосовой помощник"));
    setWindowIcon(QIcon(":/res/img/microphone.png"));
    setMinimumSize(520, 810);
    setMaximumSize(520, 810);
    resize(520, 810);
}

MainWindow::~MainWindow()
{
    delete recorder;
    delete VkSpeechRec;
    delete yaSpeechRec;
    delete assistant;
    delete dataMapper;
    delete widgetWindow;
}

void MainWindow::createUserInterface()
{
    //Кнопка с gif
    toggle_record = new QPushButton(this);
    toggle_record->setObjectName("start_record");
    movie = new QMovie(":/res/img/blue_mic.gif", "GIF", this);
    movie->setScaledSize(QSize(489,491));
    connect(toggle_record, &QPushButton::clicked, this, &MainWindow::startRecord);
    toggle_record->setIconSize(QSize(489,491));
    toggle_record->setFixedHeight(489);
    toggle_record->setFixedWidth(491);
    connect(movie, &QMovie::frameChanged, this, &MainWindow::setButtonIcon);
    movie->jumpToNextFrame();
    toggle_record->setIcon(movie->currentPixmap());

    //Остальные виджеты
    btn_commandList = new QPushButton(tr("Список доступных команд"), this);
    header_textArea = new QLabel(tr("Статус: нажмите на изображение микрофона чтобы \r\n"
                                    "активировать голосовой ввод команд"), this);
    textArea = new QTextEdit(this);
    textArea->setPlaceholderText(tr("Распознанный текст команды"));
    btn_clearTextArea = new QPushButton(tr("Очистить текстовое поле"), this);
    connect(btn_clearTextArea, &QPushButton::clicked, this, &MainWindow::clearTextArea);

    labelServiceRecognition = new QLabel(tr("Сервис распознавания речи: "), this);
    btnServiceRecognition = new SliderButton(tr("VK ASR API "), tr(" Yandex  "), this);
    connect(btnServiceRecognition, &SliderButton::currentState, this, &MainWindow::changingServiceSlider);

    labelMode = new QLabel(tr("                           "), this);
    btn_modeSelection = new SliderButton(tr(" spontaneous"), tr("neutral "), this);
    connect(btn_modeSelection, &SliderButton::currentState, this, &MainWindow::changingModeSlider);

    //Компоновка виджетов
    vertical_layout = new QVBoxLayout();
    vertical_layout->setMargin(7);
    vertical_layout->addWidget(btn_commandList, Qt::AlignCenter);
    vertical_layout->addWidget(toggle_record, Qt::AlignCenter);
    vertical_layout->addWidget(header_textArea, Qt::AlignCenter);
    vertical_layout->addWidget(textArea, Qt::AlignCenter);
    vertical_layout->addWidget(btn_clearTextArea, Qt::AlignCenter);
    layout_service = new QHBoxLayout();
    layout_service->setSpacing(10);
    layout_service->addWidget(labelServiceRecognition, Qt::AlignCenter);
    layout_service->addWidget(btnServiceRecognition, Qt::AlignCenter);
    layout_mode = new QHBoxLayout();
    layout_mode->setSpacing(10);
    layout_mode->addWidget(labelMode, Qt::AlignCenter);
    layout_mode->addWidget(btn_modeSelection, Qt::AlignCenter);
    vertical_layout->addLayout(layout_service);
    vertical_layout->addLayout(layout_mode);
    widgetWindow->setLayout(vertical_layout);
}

void MainWindow::showMessageBoxErrorCommand(QString& textMessage)
{
    QMessageBox msgBox;
    msgBox.setText(textMessage);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet("color: black");
    msgBox.button(QMessageBox::Ok)->animateClick(3000);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
}

void MainWindow::startRecord()
{
    statusRecord = !statusRecord;
    if (statusRecord) {
        movie->start();
        setServiceRecognition(serviceRecognition);
        commandRecord(true);
        header_textArea->setText(tr("Статус: ожидание голосовой команды"));
    }
    else {
        movie->stop();
        commandRecord(false);
        header_textArea->setText(tr("Статус: нажмите на изображение микрофона чтобы \r\n"
                                    "активировать голосовой ввод команд"));
    }
}

void MainWindow::setButtonIcon()
{
    toggle_record->setIcon(movie->currentPixmap());
}

void MainWindow::clearTextArea()
{
    textArea->clear();
}

//Добавить проверку наличия активного интернет-соединения при запуске приложения и вот здесь
//Перед отправкой аудио в web API распознавания речи
//Если ошибка, то выдавать сообщение: "Ошибка. Проверьте наличие интернет соединения"
//Приложение использует интернет-соединение для распознавания речи, построения маршрутов и показа погоды
void MainWindow::getPathToAudioFile(QString filePath)
{
    header_textArea->setText(tr("Статус: запущено распознавание речи"));
    if (serviceRecognition == "yandex") {
        postRequestToYandexAPI(filePath);
    }
    if (serviceRecognition == "vkAPI") {
        postRequestToVkAPI(filePath, modelRecognition);
    }
}

void MainWindow::updateStatusProcess(QString newStatus)
{
    header_textArea->setText(tr("Статус: ") + newStatus);
}

void MainWindow::speechRecognitionError(QString errorMessage)
{
    header_textArea->setText(tr("Статус: ") + errorMessage);
    recorder->deleteAudioFiles();
    showMessageBoxErrorCommand(errorMessage);
}

void MainWindow::getRecognizedSpeech(QString speechToText)
{
    header_textArea->setText(tr("Статус: распознавание успешно выполнено"));
    textArea->append(speechToText);
    recorder->deleteAudioFiles();
    requestCommand(speechToText);
}

void MainWindow::changingModeSlider(bool currentMode)
{
    if (currentMode)
        modelRecognition = "neutral";
    if (!currentMode)
        modelRecognition = "spontaneous";
}

void MainWindow::changingServiceSlider(bool currentService)
{
    btn_modeSelection->setFixedHeight(btn_modeSelection->getSizeBtn());
    labelMode->setFixedHeight(btn_modeSelection->getSizeBtn());
    if (currentService) {
        serviceRecognition = "yandex";
        btn_modeSelection->setVisible(false);
        labelMode->setText(tr("                           "));
        setServiceRecognition(serviceRecognition);
    }
    if (!currentService) {
        serviceRecognition = "vkAPI";
        btn_modeSelection->setVisible(true);
        labelMode->setText(tr("Модель распознавания речи: "));
        setServiceRecognition(serviceRecognition);
    }
}

void MainWindow::readData()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./dataDB.db");
    QVector<QString> parameters;
    if (!dataMapper->readDataSQL(db, parameters)) {
        QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (tr("Приложение не может работать без отсутствующей "
                                                                                     "базы данных токенов!")));
        msg->setStyleSheet("color: black");
        if (msg->exec()==QMessageBox::Ok) delete msg;
        this->close();
        exit(1);
    }
    sendVkSpeechToken(parameters[0]);
    sendYandexSpeechToken(parameters[1]);
    sendYandexGeoToken(parameters[2]);
    sendPathToFFMPEG(parameters[3]);
}
