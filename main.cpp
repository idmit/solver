#include "controller.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller controller;

    MainWindow *w = new MainWindow(&controller);
    w->show();

    return a.exec();
}
