#include "controller.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller controller;
    MainWindow *mainWindow = new MainWindow(&controller);

    mainWindow->show();

    return a.exec();
}
