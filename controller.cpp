#include "controller.h"
#include <QMessageBox>
#include <QPushButton>

Controller::Controller(QWidget *parent) :
    QWidget(parent)
{

}

void Controller::initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, TaskWindow *_taskWindow, Model *_model)
{
    mainWindow = _mainWindow;
    connectionWindow = _connectionWindow;
    taskWindow = _taskWindow;
    model = _model;

    QObject::connect(connectionWindow, SIGNAL(optionsSpecified()), this, SLOT(processConnectionOptions()));

    QObject::connect(model, SIGNAL(statusChanged(QString,int)), this, SIGNAL(statusChanged(QString,int)));
    QObject::connect(this, SIGNAL(statusChanged(QString,int)), mainWindow, SLOT(refreshStatus(QString,int)));

    QObject::connect(mainWindow, SIGNAL(currentTaskTypeIndexChanged(int)), this, SLOT(showTaskHistory(int)));

    QObject::connect(mainWindow, SIGNAL(processTask(int)), this, SLOT(showTaskWindow(int)));

    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), taskWindow, SLOT(allowEdit()));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), taskWindow, SLOT(hideEditButton()));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), model, SLOT(makeTaskNew()));

    QObject::connect(taskWindow, SIGNAL(solveButtonClicked(QStringList,QStringList)), this, SLOT(processTask(QStringList,QStringList)));
}

/* CONNECTION CREATION BEGIN */

void Controller::showConnectionWindow()
{
    QStringList drivers;

    model->retrieveDrivers(drivers);
    connectionWindow->refreshDrivers(drivers);

    connectionWindow->show();
    connectionWindow->setFocus();
}

void Controller::processConnectionOptions()
{
    QHash<QString, QString> options;
    QMessageBox msgBox(connectionWindow);
    bool result = false;

    connectionWindow->getOptions(options);
    model->attemptToAddConnection(options, result);

    if (result)
    {
        statusChanged(CONNECTION_SUCC, 2000);
    }
    else
    {
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setText(CONNECTION_ERR);
        msgBox.exec();
    }
    connectionWindow->setHidden(result);
    showTaskTypes(result);
}

/* CONNECTION CREATION END */

void Controller::showTaskTypes(bool connectionExists)
{
    QStringList taskTypes;

    if (!connectionExists)
        return;

    model->retrieveTaskTypes(taskTypes);
    mainWindow->refreshTaskTypesCombo(taskTypes);
}

void Controller::showTaskHistory(int taskTypeIndex)
{
    QStringList taskHistory;
    int taskTypeId = taskTypeIndex + 1;

    model->retrieveTaskHistory(taskTypeId, taskHistory);
    mainWindow->refreshTaskHistoryList(taskHistory);
}

void Controller::showSolutionMethods(int taskTypeId)
{
    QStringList solutionMethods;

    model->retrieveSolutionMethods(taskTypeId, solutionMethods);
    taskWindow->refreshSolutionMethods(solutionMethods);
}

void Controller::showTaskWindow(int taskIndexInHistory)
{
    int taskTypeIndex = 0,
        taskTypeId    = 0,
        taskNumberInHistory = taskIndexInHistory + 1,
        taskId        = 0;
    bool taskIsNew = false;
    QStringList lValues, rValues;

    mainWindow->currentTypeIndex(taskTypeIndex);
    taskTypeId = taskTypeIndex + 1;

    showSolutionMethods(taskTypeId);

    if (taskTypeId == LE_TYPE_ID)
        taskWindow->enableFirstAddButton(false);
    else if (SLAE_TYPE_ID)
        taskWindow->enableFirstAddButton(true);

    taskIsNew = taskNumberInHistory ? false : true;
    if (taskIsNew)
    {
        taskWindow->clear();
        taskWindow->appendLine();
        model->regTask(0, taskTypeId, true);
    }
    else
    {
        model->retrieveTaskFromHistory(taskId, taskTypeId, taskNumberInHistory, lValues, rValues);
        taskWindow->refreshLines(lValues, rValues);
        model->regTask(taskId, taskTypeId, false);
    }

    taskWindow->showEditButton(!taskIsNew);
    if (!taskIsNew)
        taskWindow->forbidEdit();

    taskWindow->show();
}

void Controller::showSolution(int solutionMethodId)
{
    QString solution;
    model->retrieveSolutionForProcessedTask(solution, solutionMethodId);
    taskWindow->refreshSolution(solution);
}

void Controller::removeRedundantData(QStringList &lValues, QStringList &rValues)
{
    for (int i = 0; i < lValues.size(); ++i)
    {
        while(lValues[i].endsWith(' '))
            lValues[i].chop(1);
        while(rValues[i].endsWith(' '))
            rValues[i].chop(1);
        if (lValues[i].isEmpty() || rValues[i].isEmpty())
        {
            lValues.removeAt(i);
            rValues.removeAt(i);
            i--;
        }
    }
}

void Controller::processTask(QStringList lValues, QStringList rValues)
{
    removeRedundantData(lValues, rValues);

    if (lValues.size() == 0)
    {
        QMessageBox msgBox(taskWindow);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setText("Your task is empty");
        msgBox.exec();
        return;
    }

    int solutionMethodNumberInList = 0,
            solutionMethodIndex = 0,
            solutionMethodId = 0;

    taskWindow->currentSolutionMethodIndex(solutionMethodIndex);
    solutionMethodNumberInList = solutionMethodIndex + 1;
    model->retrieveSolutionMethodFromList(solutionMethodId, solutionMethodNumberInList);

    model->solveTask(lValues, rValues, solutionMethodId);

    showSolution(solutionMethodId);

    showTaskHistory(model->processedTaskTypeId() - 1);
    taskWindow->hide();
}
