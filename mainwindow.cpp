#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *mainMenu = this->menuBar()->addMenu(QObject::tr("File"));
    mainMenu->addAction(QObject::tr("Connect to server"), parent, SLOT(showConnectionWindow()), QKeySequence(Qt::CTRL + Qt::Key_D));
    QMenu *historyMenu = this->menuBar()->addMenu(QObject::tr("History"));
    historyMenu->addAction(QObject::tr("Delete selected history item"), parent, SLOT(deleteHistoryItem()), QKeySequence(Qt::CTRL + Qt::Key_Backspace));

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

void MainWindow::currentTypeIndex(int &currentIndex)
{
    currentIndex = ui->taskTypesCombo->currentIndex();
}

void MainWindow::on_taskTypesCombo_currentIndexChanged(int index)
{
    emit currentTaskTypeIndexChanged(index);
}

void MainWindow::on_newTaskButton_clicked()
{
    emit processTask(-1);
}

void MainWindow::on_taskHistoryList_doubleClicked(const QModelIndex &index)
{
    emit processTask(index.row());
}

void MainWindow::selectedHistoryItemIndex(QVector<int> &selectedIndexes)
{
    foreach(const QModelIndex &index, ui->taskHistoryList->selectionModel()->selectedIndexes())
        selectedIndexes.append(index.row());
}
