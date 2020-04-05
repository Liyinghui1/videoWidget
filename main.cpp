#include "mainWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QDebug>
#include <QString>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::addLibraryPath("./plugins");
    QString name = QDir::currentPath();
    QPixmap appIcon(":/images/images/appIcon.png");
    a.setWindowIcon(appIcon);
    CWidget w;
    int width = QApplication::desktop()->availableGeometry().width();
    int height = QApplication::desktop()->availableGeometry().height();
    w.setGeometry((width-w.getWindowIniWidth())/2, (height-w.getWindowIniHeight())/2, w.getWindowIniWidth(), w.getWindowIniHeight());
    w.show();
//    w.raise();
//    w.activateWindow();

    return a.exec();
}

