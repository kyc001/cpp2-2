#include <QApplication>
#include <string>
#include <iostream>
#include "ui/menu.h"
#include <QtCore/QResource>

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(gameres);
    QApplication a(argc, argv);
    Menu * chick_start = new Menu();
    chick_start->show();
    return QApplication::exec();
}
