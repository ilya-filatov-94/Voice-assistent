#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    widgetWindow = new QWidget(this);
    widgetWindow->setObjectName("widgetWindow");

    //Кнопка с gif
    toggle_record = new QPushButton(this);
    toggle_record->setObjectName("start_record");
    movie = new QMovie(":/res/img/blue_mic.gif");
    movie->setScaledSize(QSize(489,491));
    connect(toggle_record, &QPushButton::clicked, this, &MainWindow::startRecord);
    toggle_record->setIconSize(QSize(489,491));
    toggle_record->setFixedHeight(489);
    toggle_record->setFixedWidth(491);
    connect(movie, &QMovie::frameChanged, this, &MainWindow::setButtonIcon);
    movie->start();
    movie->stop();

    //Остальные виджеты
    btn_commandList = new QPushButton(tr("Список доступных команд"), this);
    header_textArea = new QLabel(tr("Статус: запустите распознавание речи"), this);
    header_textArea->setAlignment(Qt::AlignLeft);
    textArea = new QTextEdit(this);
    textArea->setPlaceholderText(tr("Распознанный текст команды"));
    btn_clearTextArea = new QPushButton(tr("Очистить текстовое поле"), this);
    connect(btn_clearTextArea, &QPushButton::clicked, this, &MainWindow::clearTextArea);
    labelMode = new QLabel(tr("Модель распознавания речи: "), this);
    btn_modeSelection = new SliderButton(tr(" spontaneous"), tr("neutral "), this);
    connect(btn_modeSelection, &SliderButton::currentState, this, &MainWindow::changingStatusSlider);
    modelRecognition = "neutral";

    //Компоновка виджетов
    vertical_layout = new QVBoxLayout();
    vertical_layout->setMargin(15);
    vertical_layout->setSpacing(10);
    vertical_layout->addWidget(btn_commandList, Qt::AlignCenter);
    vertical_layout->addWidget(toggle_record, Qt::AlignCenter);
    vertical_layout->addStretch(1);
    vertical_layout->addWidget(header_textArea, Qt::AlignCenter);
    vertical_layout->addWidget(textArea, Qt::AlignCenter);
    vertical_layout->addWidget(btn_clearTextArea, Qt::AlignCenter);
    layout_mode = new QHBoxLayout();
    layout_mode->setSpacing(10);
    layout_mode->addWidget(labelMode, Qt::AlignCenter);
    layout_mode->addWidget(btn_modeSelection, Qt::AlignCenter);
    vertical_layout->addLayout(layout_mode);

    recorder = new AudioRecorder(this);
    statusRecord = false;
    connect(this, &MainWindow::commandRecord, recorder, &AudioRecorder::toggleRecord);
    connect(recorder, &AudioRecorder::sendPathToAudioFile, this, &MainWindow::getPathToAudioFile);

    request_manager = new NetworkRequests(this);
    connect(this, &MainWindow::startRequestsToAPI, request_manager, &NetworkRequests::requestUploadUrlAPI);
    connect(request_manager, &NetworkRequests::sendStatusProcess, this, &MainWindow::updateStatusProcess);
    connect(request_manager, &NetworkRequests::sendRecognizedSpeech, this, &MainWindow::getRecognizedSpeech);

    yaSpeech = new RecognizedSpeechYandexAPI(this);
    connect(this, &MainWindow::postYaAPI, yaSpeech, &RecognizedSpeechYandexAPI::postAudioToAPI);

    assistant = new CommandsExec(this);
    connect(this, &MainWindow::requestCommand, assistant, &CommandsExec::choose_action);

    widgetWindow->setLayout(vertical_layout);
    setCentralWidget(widgetWindow);
    setWindowTitle(tr("Распознавание речи"));
    setMinimumSize(520, 800);
    setMaximumSize(520, 800);
    resize(520, 800);
}

MainWindow::~MainWindow()
{
    delete recorder;
    delete request_manager;
    delete assistant;
    delete widgetWindow;
}

void MainWindow::startRecord()
{
    statusRecord = !statusRecord;
    if (statusRecord) {
        movie->start();
        commandRecord(true);
        header_textArea->setText(tr("Статус: аудиозапись речи"));
    }
    else {
        movie->stop();
        commandRecord(false);
        header_textArea->setText(tr("Статус: запустите распознавание речи"));
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

void MainWindow::getPathToAudioFile(QString filePath)
{
    header_textArea->setText(tr("Статус: запущено распознавание речи"));

    //Для vkAPI
//    startRequestsToAPI(filePath, modelRecognition);

    //Для YandexSpeechKit
//    postYaAPI(filePath);
}

void MainWindow::updateStatusProcess(QString newStatus)
{
    header_textArea->setText(tr("Статус: ") + newStatus);
}

void MainWindow::getRecognizedSpeech(QString speechToText)
{
    header_textArea->setText(tr("Статус: распознавание успешно выполнено"));
    textArea->append(speechToText);
    requestCommand(speechToText);   //Отправка команды на выполнение
}

void MainWindow::changingStatusSlider(bool currentMode)
{
    if (currentMode)
        modelRecognition = "neutral";
    if (!currentMode)
        modelRecognition = "spontaneous";
}
