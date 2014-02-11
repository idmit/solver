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

void MainWindow::refreshStatus(QString status, int timeout)
{
    this->statusBar()->showMessage(status, timeout);
}

void MainWindow::refreshTaskTypesCombo(QStringList &taskTypes)
{
    ui->taskTypesCombo->clear();
    ui->taskTypesCombo->addItems(taskTypes);
}

void MainWindow::refreshTaskHistoryList(QStringList &taskHistory)
{
    ui->taskHistoryList->clear();
    ui->taskHistoryList->addItems(taskHistory);
}

void MainWindow::on_taskTypesCombo_currentIndexChanged(int index)
{
    emit currentTaskTypeIndexChanged(index);
}

void MainWindow::on_newTaskButton_clicked()
{
    emit newTaskButtonClicked();
}
