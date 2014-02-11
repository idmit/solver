#include "taskwindow.h"
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);

    setWindowTitle(TASK_WINDOW_TITLE);
}

TaskWindow::~TaskWindow()
{
    delete ui;
}

void TaskWindow::refreshSolutionMethods(QStringList &solutionMethods)
{
    ui->solutionMethodsCombo->clear();
    ui->solutionMethodsCombo->addItems(solutionMethods);
}

void TaskWindow::on_pushButton_clicked()
{
    this->hide();
}
