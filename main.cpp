#include "controller.h"
#include "mainwindow.h"
#include "connectionwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller controller;
    Model model;
    MainWindow *mainWindow = new MainWindow(&controller);
    ConnectionWindow *connectionWindow = new ConnectionWindow(mainWindow);

    controller.initialize(mainWindow, connectionWindow, &model);

    mainWindow->show();
    controller.showConnectionWindow();

    return a.exec();
}
