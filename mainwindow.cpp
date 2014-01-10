#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *mainMenu = this->menuBar()->addMenu(QObject::tr("File"));
    mainMenu->addAction(QObject::tr("Connect to server"), parent, SLOT(showConnectionWindow()));
    mainMenu->addAction(QObject::tr("Exit"), parent, SLOT(quitApp()));

    setWindowTitle(MAIN_WINDOW_HEADER);
}

MainWindow::~MainWindow()
{
    delete ui;
}
