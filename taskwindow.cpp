#include "taskwindow.h"
#include <QLineEdit>
#include <QLabel>

#include "ui_taskwindow.h"

#define LEFT_VALUES_INDEX 0
#define EQ_SIGN_INDEX 1
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

void TaskWindow::refreshSolutionMethodsCombo(QStringList const &solutionMethods)
{
    ui->solutionMethodsCombo->clear();
    ui->solutionMethodsCombo->addItems(solutionMethods);
}

void TaskWindow::addLineAtIndex(int index, QString const &lValue, QString const &rValue)
{
    QLineEdit *leftSide = new QLineEdit(lValue), *rightSide = new QLineEdit(rValue);
    QPushButton *add = new QPushButton("+"), *remove = new QPushButton("-");
    QLabel *eq = new QLabel("=");

    add->setObjectName(QString::number(index));
    remove->setObjectName(QString::number(index));

    QObject::connect(add, SIGNAL(clicked()), this, SLOT(addLineBelowClicked()));
    QObject::connect(remove, SIGNAL(clicked()), this, SLOT(removeClickedLine()));

    ui->taskContentLayout->addWidget(leftSide, index, LEFT_VALUES_INDEX);
    ui->taskContentLayout->addWidget(eq, index, EQ_SIGN_INDEX);
    ui->taskContentLayout->addWidget(rightSide, index, RIGHT_VALUES_INDEX);
    ui->taskContentLayout->addWidget(add, index, ADD_BUTTON_INDEX);
    ui->taskContentLayout->addWidget(remove, index, REMOVE_BUTTON_INDEX);

    nextEmptyRow += 1;
    leftSide->setFocus();

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

void TaskWindow::addLineBelowClicked()
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

void TaskWindow::removeClickedLine()
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

void TaskWindow::clearCoefficientsGroupBox()
{
    for (int i = 0; i < nextEmptyRow; ++i)
    {
        removeLineAtIndex(i);
    }

    nextEmptyRow = 0;
}

void TaskWindow::enableAddButtonAtFirstLine(bool enabled)
{
    firstAddButtonDisabled = !enabled;
}

void TaskWindow::generateLines(QStringList const &lValues, QStringList const &rValues)
{
    clearCoefficientsGroupBox();

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

void TaskWindow::on_editButton_clicked()
{
    enableCreationMode();
}

void TaskWindow::on_closeButton_clicked()
{
    this->hide();
}

void TaskWindow::selectedSolutionMethodsComboIndex(int &solutionMethodIndex) const
{
    solutionMethodIndex = ui->solutionMethodsCombo->currentIndex();
}

void TaskWindow::on_createButton_clicked()
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

    emit createButtonClicked(lValues, rValues);
}

void TaskWindow::enableSolutionMode()
{
    ui->scrollArea->setEnabled(false);
    ui->editButton->setVisible(true);
    ui->solutionMethodsCombo->setVisible(true);
    ui->solutionMethodsLabel->setVisible(true);
    ui->solveButton->setVisible(true);
    ui->createButton->setVisible(false);
}

void TaskWindow::enableCreationMode()
{
    ui->scrollArea->setEnabled(true);
    ui->editButton->setVisible(false);
    ui->solutionMethodsCombo->setVisible(false);
    ui->solutionMethodsLabel->setVisible(false);
    ui->solveButton->setVisible(false);
    ui->createButton->setVisible(true);
}
