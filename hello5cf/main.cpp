#include <QCoreApplication>
#include "maincontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MainController *mainCtrl = new MainController();
    mainCtrl->initialize();
    return a.exec();
}
