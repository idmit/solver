#include "taskwindow.h"
#include <QLineEdit>
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow),
    nextEmptyRow(0)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);
    setWindowTitle(TASK_WINDOW_TITLE);

    appendLine();
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

void TaskWindow::appendLine()
{
    QLineEdit *leftSide = new QLineEdit(), *rightSide = new QLineEdit();
    QPushButton *add = new QPushButton("+"), *remove = new QPushButton("-");

    add->setObjectName(QString::number(nextEmptyRow));

    QObject::connect(remove, SIGNAL(clicked()), leftSide, SLOT(deleteLater()));
    QObject::connect(remove, SIGNAL(clicked()), rightSide, SLOT(deleteLater()));
    QObject::connect(remove, SIGNAL(clicked()), add, SLOT(deleteLater()));
    QObject::connect(remove, SIGNAL(clicked()), remove, SLOT(deleteLater()));

    ui->taskContentLayout->addWidget(leftSide, nextEmptyRow, 0);
    ui->taskContentLayout->addWidget(rightSide, nextEmptyRow, 1);
    ui->taskContentLayout->addWidget(add, nextEmptyRow, 2);
    ui->taskContentLayout->addWidget(remove, nextEmptyRow, 3);

    nextEmptyRow += 1;
}

void TaskWindow::on_solveButton_clicked()
{
    this->hide();
}
