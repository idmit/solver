#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *mainMenu = this->menuBar()->addMenu(QObject::tr(FIRST_MENU_ITEM));
    mainMenu->addAction(QObject::tr(CONNECT_MENU_ACTION), parent, SLOT(showConnectionWindow()), QKeySequence(Qt::CTRL + Qt::Key_D));

    QMenu *historyMenu = this->menuBar()->addMenu(QObject::tr(SECOND_MENU_ITEM));
    historyMenu->addAction(QObject::tr(DELETE_MENU_ITEM), parent, SLOT(deleteHistoryItem()), QKeySequence(Qt::CTRL + Qt::Key_Backspace));

    setWindowTitle(MAIN_WINDOW_TITLE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshStatusBar(QString const &status, int timeout)
{
    this->statusBar()->showMessage(status, timeout);
}

void MainWindow::refreshTaskTypesCombo(QStringList const &taskTypes)
{
    ui->taskTypesCombo->clear();
    ui->taskTypesCombo->addItems(taskTypes);
}

void MainWindow::refreshTaskHistoryList(QStringList const &taskHistory)
{
    ui->taskHistoryList->clear();
    ui->taskHistoryList->addItems(taskHistory);
}

void MainWindow::selectedTypesComboIndex(int &selectedIndex) const
{
    selectedIndex = ui->taskTypesCombo->currentIndex();
}

void MainWindow::on_taskTypesCombo_currentIndexChanged(int index)
{
    emit differentTaskTypeChosen(index);
}

void MainWindow::on_newTaskButton_clicked()
{
    emit processTask(-1);
}

void MainWindow::on_taskHistoryList_doubleClicked(const QModelIndex &index)
{
    emit processTask(index.row());
}

void MainWindow::selectedHistoryListIndexes(QVector<int> &selectedIndexes) const
{
    foreach(const QModelIndex &index, ui->taskHistoryList->selectionModel()->selectedIndexes())
        selectedIndexes.append(index.row());
}

bool MainWindow::showAllCheckBoxChecked()
{
    return ui->showAllCheckBox->isChecked();
}

void MainWindow::on_showAllCheckBox_stateChanged(int arg1)
{
    emit showAllCheckBoxChanged(ui->taskTypesCombo->currentIndex());
}
