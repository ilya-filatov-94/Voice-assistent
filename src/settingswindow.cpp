#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QWidget(parent)
{
    widget = new QWidget(this);
    widget->setObjectName("widgetWindow");
    dataMapper = new Datamapper(this);

    backBtn = new QPushButton(tr("Назад"), this);
    connect(backBtn, &QPushButton::clicked, [this](){
        emit showMainWindow(false);
    });

    btn_commandList = new QPushButton(tr("Список доступных команд"), this);
    loadReference();
    connect(btn_commandList, &QPushButton::clicked, this, &SettingsWindow::showReference);

    createInputLines();
    labelVkToken = new QLabel(tr("Ключ авторизации для VK ASR API"), this);
    inputLine[0]->setPlaceholderText(tr("Введите ключ авторизации для VK API"));

    labelYandexSpeechToken = new QLabel(tr("Ключ авторизации для Yandex SpeechKit API"), this);
    inputLine[1]->setPlaceholderText(tr("Введите ключ авторизации Yandex SpeechKit API"));

    labelYandexGeoToken = new QLabel(tr("Ключ авторизации для Yandex GeoCoder API"), this);
    inputLine[2]->setPlaceholderText(tr("Введите ключ авторизации Yandex GeoCoder API"));

    labelPathFfmpeg = new QLabel(tr("Абсолютный путь до конвертора аудио"), this);
    inputLine[3]->setPlaceholderText(tr("Введите путь до ffmpeg на диске"));

    saveBtn = new QPushButton(tr("Сохранить"), this);
    connect(saveBtn, &QPushButton::clicked, this, &SettingsWindow::writeSettings);

    vlayout = new QVBoxLayout();
    vlayout->setSpacing(20);
    vlayout->setContentsMargins(5, 7, 5, 7);
    vlayout->addWidget(backBtn, Qt::AlignVCenter);
    vlayout->addWidget(btn_commandList, Qt::AlignVCenter);
    vlayout->addStretch(1);
    vlayout->addWidget(labelVkToken);
    vlayout->addLayout(hLayoutInputs[0]);
    vlayout->addWidget(labelYandexSpeechToken);
    vlayout->addLayout(hLayoutInputs[1]);
    vlayout->addWidget(labelYandexGeoToken);
    vlayout->addLayout(hLayoutInputs[2]);
    vlayout->addWidget(labelPathFfmpeg);
    vlayout->addLayout(hLayoutInputs[3]);
    vlayout->addStretch(3);
    vlayout->addWidget(saveBtn, Qt::AlignVCenter);
    widget->setLayout(vlayout);

    widget->setMinimumSize(520, 810);
    widget->setMaximumSize(520, 810);
}

SettingsWindow::~SettingsWindow()
{
    delete widget;
}

void SettingsWindow::changeVisibleLine(int id)
{
    if (QLineEdit::Password == inputLine[id]->echoMode())
    {
        inputLine[id]->setEchoMode(QLineEdit::Normal);
        visibleLine[id]->setIcon(QIcon("://res/img/lock_open.png"));
        visibleLine[id]->setIconSize(QSize(35,35));
    }
    else
    {
        inputLine[id]->setEchoMode(QLineEdit::Password);
        visibleLine[id]->setIcon(QIcon("://res/img/lock_closed.png"));
        visibleLine[id]->setIconSize(QSize(35,35));
    }
}

void SettingsWindow::createInputLines()
{
    const int sizeArrayLineEdit = sizeof(inputLine)/sizeof(*inputLine);
    for (int i = 0; i < sizeArrayLineEdit; i++) {
        inputLine[i] = new QLineEdit(this);
        inputLine[i]->setEchoMode(QLineEdit::Password);
        visibleLine[i] = new QPushButton(this);
        visibleLine[i]->setFixedHeight(35);
        visibleLine[i]->setFixedWidth(35);
        visibleLine[i]->setIcon(QIcon("://res/img/lock_closed.png"));
        visibleLine[i]->setIconSize(QSize(35,35));
        hLayoutInputs[i] = new QHBoxLayout();
        hLayoutInputs[i]->setMargin(5);
        hLayoutInputs[i]->setSpacing(5);
        hLayoutInputs[i]->addWidget(inputLine[i]);
        hLayoutInputs[i]->addWidget(visibleLine[i]);
        connect(visibleLine[i], &QPushButton::clicked, [this, i](){
            changeVisibleLine(i);
        });
    }
}

void SettingsWindow::loadReference()
{
    QFile file(":/res/reference.html");
    if (file.open(QIODevice::ReadOnly)) {
        temporaryFile.setFileTemplate("XXXXXX.html");
        if (temporaryFile.open()) {
            temporaryFile.write(file.readAll());
            file.close();
        }
    }
}

void SettingsWindow::showReference()
{
    desktopService.openUrl(QUrl::fromLocalFile(temporaryFile.fileName()));
}

void SettingsWindow::writeSettings()
{
    db = QSqlDatabase::database();
    bool error = false;
    const int sizeArrayLineEdit = sizeof(inputLine)/sizeof(*inputLine);
    for (int i = 0; i < sizeArrayLineEdit; i++) {
        if (inputLine[i]->text().isEmpty()) {
            error = true;
        }
    }
    if (error) {
        QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (tr("Ни одно поле ввода не может быть пустым!")));
        msg->setStyleSheet("color: black");
        if (msg->exec()==QMessageBox::Ok) delete msg;
    }
    else {
        QSqlQuery query;
        if(db.open()) {
            int id = 0;
            QString parameterValue;
            QVector<QString> parameters;
            QString namesOfParameters[sizeArrayLineEdit]={"vkSpeechToken", "yandexSpeechToken", "yandexGeoToken", "pathToFFMPEG"};
            QString namesOfErrorWriteData[sizeArrayLineEdit]={"Ошибка сохранения token VK API",
                                                              "Ошибка сохранения token Yandex Speech",
                                                              "Ошибка сохранения token Yandex GeoCoder",
                                                              "Ошибка сохранения пути до FFMPEG"};
            for (int i = 0; i < sizeArrayLineEdit; i++) {
                parameterValue = inputLine[i]->text();
                parameters << parameterValue;
                if (namesOfParameters[i] == "pathToFFMPEG") {
                    parameterValue += " ";
                }
                id = i + 1;
                if (!dataMapper->writeDataSQL(db, query, id, namesOfParameters[i], parameterValue)) {
                    QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (namesOfErrorWriteData[i]));
                    msg->setStyleSheet("color: black");
                    if (msg->exec()==QMessageBox::Ok) delete msg;
                    error = true;
                    break;
                }
            }
            if (!error) emit settingsSavedSuccessfully(parameters);
        }
    }
}

void SettingsWindow::getSettingsData(QVector<QString> &parameters)
{
    for (int i = 0; i < parameters.size(); i++) {
        inputLine[i]->setText(parameters.at(i));
    }
}
