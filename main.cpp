#include "controller.h"
#include "mainwindow.h"
#include "connectionwindow.h"
#include "taskwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller controller;
    Model model;
    MainWindow *mainWindow = new MainWindow(&controller);
    ConnectionWindow *connectionWindow = new ConnectionWindow(mainWindow);
    TaskWindow *taskWindow = new TaskWindow(mainWindow);

    controller.initialize(mainWindow, connectionWindow, taskWindow, &model);

    mainWindow->show();
    controller.showConnectionWindow();

    return a.exec();
}
