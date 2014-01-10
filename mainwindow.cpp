#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *mainMenu = this->menuBar()->addMenu(QObject::tr("File"));
    mainMenu->addAction(QObject::tr("Connect to server"), parent, SLOT(showConnectionWindow()), QKeySequence(Qt::CTRL + Qt::Key_D));

    setWindowTitle(MAIN_WINDOW_TITLE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEnabledInputs(bool enable)
{
    ui->taskHistoryList->setEnabled(enable);
    ui->taskTypesCombo->setEnabled(enable);
    ui->newTaskButton->setEnabled(enable);
}

void MainWindow::refreshStatus(QString status, int timeout)
{
    this->statusBar()->showMessage(status, timeout);
}
