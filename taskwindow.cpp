#include "taskwindow.h"
#include <QLineEdit>
#include <QLabel>

#include "ui_taskwindow.h"

#define LEFT_VALUES_INDEX 0
#define RIGHT_VALUES_INDEX 2
#define ADD_BUTTON_INDEX 3
#define REMOVE_BUTTON_INDEX 4
#define WIDGETS_IN_ROW 5

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow),
    nextEmptyRow(0),
    firstAddButtonDisabled(false)
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

void TaskWindow::addLineAtIndex(int index, QString lValue, QString rValue)
{
    QLineEdit *leftSide = new QLineEdit(lValue), *rightSide = new QLineEdit(rValue);
    QPushButton *add = new QPushButton("+"), *remove = new QPushButton("-");
    QLabel *eq = new QLabel("=");

    add->setObjectName(QString::number(index));
    remove->setObjectName(QString::number(index));

    QObject::connect(add, SIGNAL(clicked()), this, SLOT(addLine()));
    QObject::connect(remove, SIGNAL(clicked()), this, SLOT(removeLine()));

    ui->taskContentLayout->addWidget(leftSide, index, LEFT_VALUES_INDEX);
    ui->taskContentLayout->addWidget(eq, index, 1);
    ui->taskContentLayout->addWidget(rightSide, index, RIGHT_VALUES_INDEX);
    ui->taskContentLayout->addWidget(add, index, ADD_BUTTON_INDEX);
    ui->taskContentLayout->addWidget(remove, index, REMOVE_BUTTON_INDEX);

    nextEmptyRow += 1;

    /* Doesn't work another way */
    if (index == 0)
    {
        remove->setEnabled(false);
        add->setEnabled(!firstAddButtonDisabled);
    }
    if (index != 0)
        ui->taskContentLayout->itemAtPosition(0, REMOVE_BUTTON_INDEX)->widget()->setEnabled(true);
}

void TaskWindow::removeLineAtIndex(int index)
{
    for (int j = 0; j < WIDGETS_IN_ROW; ++j)
    {
        ui->taskContentLayout->itemAtPosition(index, j)->widget()->deleteLater();
    }
}

void TaskWindow::appendLine()
{
    addLineAtIndex(nextEmptyRow);
}

void TaskWindow::addLine()
{
    int clickedPosition = QObject::sender()->objectName().toInt();

    for (int i = nextEmptyRow - 1; i > clickedPosition; --i)
    {
        QWidget *addButtonWidget = ui->taskContentLayout->itemAtPosition(i, ADD_BUTTON_INDEX)->widget();
        QWidget *removeButtonWidget = ui->taskContentLayout->itemAtPosition(i, REMOVE_BUTTON_INDEX)->widget();
        int nameAsInt = addButtonWidget->objectName().toInt();

        addButtonWidget->setObjectName(QString::number(nameAsInt + 1));
        removeButtonWidget->setObjectName(QString::number(nameAsInt + 1));

        for (int j = 0; j < WIDGETS_IN_ROW; ++j)
        {
            ui->taskContentLayout->addWidget(ui->taskContentLayout->itemAtPosition(i, j)->widget(), i + 1, j);
        }
    }

    addLineAtIndex(clickedPosition + 1);
}

void TaskWindow::removeLine()
{
    int clickedPosition = QObject::sender()->objectName().toInt();
    QWidget *toBeDisabled = ui->taskContentLayout->itemAtPosition(1, REMOVE_BUTTON_INDEX)->widget();

    removeLineAtIndex(clickedPosition);

    for (int i = clickedPosition + 1; i < nextEmptyRow; ++i)
    {
        QWidget *addButtonWidget = ui->taskContentLayout->itemAtPosition(i, ADD_BUTTON_INDEX)->widget();
        QWidget *removeButtonWidget = ui->taskContentLayout->itemAtPosition(i, REMOVE_BUTTON_INDEX)->widget();
        int nameAsInt = addButtonWidget->objectName().toInt();
        addButtonWidget->setObjectName(QString::number(nameAsInt - 1));
        removeButtonWidget->setObjectName(QString::number(nameAsInt - 1));

        for (int j = 0; j < WIDGETS_IN_ROW; ++j)
        {
            ui->taskContentLayout->addWidget(ui->taskContentLayout->itemAtPosition(i, j)->widget(), i - 1, j);
        }
    }

    nextEmptyRow -= 1;

    /* Doesn't work another way */
    if (clickedPosition == 0)
        toBeDisabled->setEnabled(nextEmptyRow != 1);
    if (clickedPosition == 1)
        ui->taskContentLayout->itemAtPosition(0, REMOVE_BUTTON_INDEX)->widget()->setEnabled(nextEmptyRow != 1);
}

void TaskWindow::clear()
{
    for (int i = 0; i < nextEmptyRow; ++i)
    {
        removeLineAtIndex(i);
    }

    nextEmptyRow = 0;
}

void TaskWindow::enableFirstAddButton(bool en)
{
    firstAddButtonDisabled = !en;
}

void TaskWindow::refreshLines(QStringList lValues, QStringList rValues)
{
    clear();

    for (int i = 0; i < lValues.size(); ++i)
    {
        addLineAtIndex(i, lValues[i], rValues[i]);
    }
}

void TaskWindow::on_solveButton_clicked()
{
    QStringList lValues, rValues;

    for (int i = 0; i < nextEmptyRow; ++i)
    {
        QLineEdit *lSide = 0, *rSide = 0;
        lSide = dynamic_cast<QLineEdit *>(ui->taskContentLayout->itemAtPosition(i, LEFT_VALUES_INDEX)->widget());
        lValues << lSide->text();
        rSide = dynamic_cast<QLineEdit *>(ui->taskContentLayout->itemAtPosition(i, RIGHT_VALUES_INDEX)->widget());
        rValues << rSide->text();
    }

    emit solveButtonClicked(lValues, rValues);
}

void TaskWindow::forbidEdit()
{
    ui->scrollArea->setEnabled(false);
}

void TaskWindow::allowEdit()
{
    ui->scrollArea->setEnabled(true);
}

void TaskWindow::showEditButton(bool en)
{
    ui->editButton->setVisible(en);
}

void TaskWindow::on_editButton_clicked()
{
    emit editButtonClicked();
}

void TaskWindow::hideEditButton()
{
    showEditButton(false);
}

void TaskWindow::on_closeButton_clicked()
{
    this->hide();
}

void TaskWindow::currentSolutionMethodIndex(int &solutionMethodIndex)
{
    solutionMethodIndex = ui->solutionMethodsCombo->currentIndex();
}
