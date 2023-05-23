#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QWidget(parent)
{
    widget = new QWidget(this);
    widget->setObjectName("widgetWindow");
    dataMapper = new Datamapper(this);

    backBtn = new QPushButton(tr("Назад"), this);
    connect(backBtn, &QPushButton::clicked, this, &SettingsWindow::switchWindow);

    btn_commandList = new QPushButton(tr("Список доступных команд"), this);
    loadReference();
    connect(btn_commandList, &QPushButton::clicked, this, &SettingsWindow::showReference);

    createInputLines();
    labelVkToken = new QLabel(tr("Ключ авторизации для VK ASR API"), this);
    inputLine[0]->setPlaceholderText(tr("Введите ключ авторизации для VK API"));
    connect(visibleLine[0], &QPushButton::clicked, this, &SettingsWindow::changeVisibleLine1);

    labelYandexSpeechToken = new QLabel(tr("Ключ авторизации для Yandex SpeechKit API"), this);
    inputLine[1]->setPlaceholderText(tr("Введите ключ авторизации Yandex SpeechKit API"));
    connect(visibleLine[1], &QPushButton::clicked, this, &SettingsWindow::changeVisibleLine2);

    labelYandexGeoToken = new QLabel(tr("Ключ авторизации для Yandex GeoCoder API"), this);
    inputLine[2]->setPlaceholderText(tr("Введите ключ авторизации Yandex GeoCoder API"));
    connect(visibleLine[2], &QPushButton::clicked, this, &SettingsWindow::changeVisibleLine3);

    labelPathFfmpeg = new QLabel(tr("Абсолютный путь до конвертора аудио"), this);
    inputLine[3]->setPlaceholderText(tr("Введите путь до ffmpeg на диске"));
    connect(visibleLine[3], &QPushButton::clicked, this, &SettingsWindow::changeVisibleLine4);

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

void SettingsWindow::createInputLines()
{
    for (int i = 0; i < 4; i++) {
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
    }
}

void SettingsWindow::changeVisibleLine1()
{
    if (QLineEdit::Password == inputLine[0]->echoMode())
    {
        inputLine[0]->setEchoMode(QLineEdit::Normal);
        visibleLine[0]->setIcon(QIcon("://res/img/lock_open.png"));
        visibleLine[0]->setIconSize(QSize(35,35));
    }
    else
    {
        inputLine[0]->setEchoMode(QLineEdit::Password);
        visibleLine[0]->setIcon(QIcon("://res/img/lock_closed.png"));
        visibleLine[0]->setIconSize(QSize(35,35));
    }
}

void SettingsWindow::changeVisibleLine2()
{
    if (QLineEdit::Password == inputLine[1]->echoMode())
    {
        inputLine[1]->setEchoMode(QLineEdit::Normal);
        visibleLine[1]->setIcon(QIcon("://res/img/lock_open.png"));
        visibleLine[1]->setIconSize(QSize(35,35));
    }
    else
    {
        inputLine[1]->setEchoMode(QLineEdit::Password);
        visibleLine[1]->setIcon(QIcon("://res/img/lock_closed.png"));
        visibleLine[1]->setIconSize(QSize(35,35));
    }
}

void SettingsWindow::changeVisibleLine3()
{
    if (QLineEdit::Password == inputLine[2]->echoMode())
    {
        inputLine[2]->setEchoMode(QLineEdit::Normal);
        visibleLine[2]->setIcon(QIcon("://res/img/lock_open.png"));
        visibleLine[2]->setIconSize(QSize(35,35));
    }
    else
    {
        inputLine[2]->setEchoMode(QLineEdit::Password);
        visibleLine[2]->setIcon(QIcon("://res/img/lock_closed.png"));
        visibleLine[2]->setIconSize(QSize(35,35));
    }
}

void SettingsWindow::changeVisibleLine4()
{
    if (QLineEdit::Password == inputLine[3]->echoMode())
    {
        inputLine[3]->setEchoMode(QLineEdit::Normal);
        visibleLine[3]->setIcon(QIcon("://res/img/lock_open.png"));
        visibleLine[3]->setIconSize(QSize(35,35));
    }
    else
    {
        inputLine[3]->setEchoMode(QLineEdit::Password);
        visibleLine[3]->setIcon(QIcon("://res/img/lock_closed.png"));
        visibleLine[3]->setIconSize(QSize(35,35));
    }
}

void SettingsWindow::switchWindow()
{
    emit showMainWindow(false);
}

void SettingsWindow::showReference()
{
    desktopService.openUrl(QUrl::fromLocalFile(temporaryFile.fileName()));
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

void SettingsWindow::writeSettings()
{
    db = QSqlDatabase::database();
    bool error = false;
    for (int i = 0; i < 4; i++) {
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
        QString parameterName, parameterValue;
        if(db.open()) {
            parameterName = "vkSpeechToken";
            parameterValue = inputLine[0]->text();
            if (!dataMapper->writeDataSQL(db, query, 1, parameterName, parameterValue)) {
                QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (tr("Ошибка сохранения token VK API")));
                msg->setStyleSheet("color: black");
                if (msg->exec()==QMessageBox::Ok) delete msg;
            }

            parameterName = "yandexSpeechToken";
            parameterValue = inputLine[1]->text();
            if (!dataMapper->writeDataSQL(db, query, 2, parameterName, parameterValue)) {
                QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (tr("Ошибка сохранения token Yandex Speech")));
                msg->setStyleSheet("color: black");
                if (msg->exec()==QMessageBox::Ok) delete msg;
            }

            parameterName = "yandexGeoToken";
            parameterValue = inputLine[2]->text();
            if (!dataMapper->writeDataSQL(db, query, 3, parameterName, parameterValue)) {
                QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (tr("Ошибка сохранения token Yandex GeoCoder")));
                msg->setStyleSheet("color: black");
                if (msg->exec()==QMessageBox::Ok) delete msg;
            }

            parameterName = "pathToFFMPEG";
            parameterValue = inputLine[3]->text();
            parameterValue += " ";
            if (!dataMapper->writeDataSQL(db, query, 4, parameterName, parameterValue)) {
                QMessageBox* msg = new QMessageBox(QMessageBox::Critical, tr("Ошибка!"), (tr("Ошибка сохранения пути до FFMPEG")));
                msg->setStyleSheet("color: black");
                if (msg->exec()==QMessageBox::Ok) delete msg;
            }
        }
    }
}

void SettingsWindow::getSettingsData(QString par1, QString par2, QString par3, QString par4)
{
    inputLine[0]->setText(par1);
    inputLine[1]->setText(par2);
    inputLine[2]->setText(par3);
    inputLine[3]->setText(par4);
}
