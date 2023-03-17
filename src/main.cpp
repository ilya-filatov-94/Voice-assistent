#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QLatin1String>
#include <QFile>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile StyleSheetFile(":/res/style1.qss");
    if (StyleSheetFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QString styleSheet=QLatin1String(StyleSheetFile.readAll());
        a.setStyleSheet(styleSheet);
        StyleSheetFile.close();
    }
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();

    return a.exec();
}
