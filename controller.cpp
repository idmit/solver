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

    QObject::connect(this, SIGNAL(retrieveDrivers(QStringList&)), model, SLOT(drivers(QStringList&)));
    QObject::connect(this, SIGNAL(displayDrivers(QStringList&)), connectionWindow, SLOT(refreshDrivers(QStringList&)));

    QObject::connect(connectionWindow, SIGNAL(optionsSpecified()), this, SLOT(processConnectionOptions()));
    QObject::connect(this, SIGNAL(attemptToConnect(QHash<QString,QString>&,bool&)), model, SLOT(attemptToAddConnection(QHash<QString,QString>&,bool&)));

    QObject::connect(model, SIGNAL(statusChanged(QString,int)), this, SIGNAL(statusChanged(QString,int)));
    QObject::connect(this, SIGNAL(statusChanged(QString,int)), mainWindow, SLOT(refreshStatus(QString,int)));

    QObject::connect(this, SIGNAL(retrieveTaskTypes(QStringList&)), model, SLOT(taskTypes(QStringList&)));
    QObject::connect(this, SIGNAL(displayTaskTypes(QStringList&)), mainWindow, SLOT(refreshTaskTypesCombo(QStringList&)));

    QObject::connect(mainWindow, SIGNAL(currentTaskTypeIndexChanged(int)), this, SLOT(showTaskHistory(int)));
    QObject::connect(this, SIGNAL(retrieveTaskHistory(int,QStringList&)), model, SLOT(taskHistory(int,QStringList&)));
    QObject::connect(this, SIGNAL(displayTaskHistory(QStringList&)), mainWindow, SLOT(refreshTaskHistoryList(QStringList&)));

    QObject::connect(mainWindow, SIGNAL(processTask(int)), this, SLOT(showTaskWindow(int)));

    QObject::connect(this, SIGNAL(retrieveSolutionMethods(int,QStringList&)), model, SLOT(solutionMethods(int,QStringList&)));
    QObject::connect(this, SIGNAL(displaySolutionMethods(QStringList&)), taskWindow, SLOT(refreshSolutionMethods(QStringList&)));

    QObject::connect(this, SIGNAL(retrieveTaskFromHistory(int&,int,int,QStringList&,QStringList&)), model, SLOT(taskFromHistory(int&,int,int,QStringList&,QStringList&)));
    QObject::connect(this, SIGNAL(displayTaskFromHistory(QStringList,QStringList)), taskWindow, SLOT(refreshLines(QStringList,QStringList)));

    QObject::connect(this, SIGNAL(regTask(int,int,bool)), model, SLOT(regTask(int,int,bool)));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), taskWindow, SLOT(allowEdit()));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), taskWindow, SLOT(hideEditButton()));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), model, SLOT(makeTaskNew()));

    QObject::connect(taskWindow, SIGNAL(solveButtonClicked(QStringList,QStringList)), this, SLOT(processTask(QStringList,QStringList)));
}

/* CONNECTION CREATION BEGIN */

void Controller::showConnectionWindow()
{
    QStringList drivers;

    emit retrieveDrivers(drivers);
    emit displayDrivers(drivers);

    connectionWindow->show();
    connectionWindow->setFocus();
}

void Controller::processConnectionOptions()
{
    QHash<QString, QString> options;
    QMessageBox msgBox(connectionWindow);
    bool result = false;

    connectionWindow->getOptions(options);
    emit attemptToConnect(options, result);

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

    emit retrieveTaskTypes(taskTypes);
    emit displayTaskTypes(taskTypes);
}

void Controller::showTaskHistory(int taskTypeIndex)
{
    QStringList taskHistory;
    int taskTypeId = taskTypeIndex + 1;

    emit retrieveTaskHistory(taskTypeId, taskHistory);
    emit displayTaskHistory(taskHistory);
}

void Controller::showSolutionMethods(int taskTypeId)
{
    QStringList solutionMethods;

    emit retrieveSolutionMethods(taskTypeId, solutionMethods);
    emit displaySolutionMethods(solutionMethods);
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
        emit regTask(0, taskTypeId, true);
    }
    else
    {
        emit retrieveTaskFromHistory(taskId, taskTypeId, taskNumberInHistory, lValues, rValues);
        emit displayTaskFromHistory(lValues, rValues);
        emit regTask(taskId, taskTypeId, false);
    }

    taskWindow->showEditButton(!taskIsNew);
    if (!taskIsNew)
        taskWindow->forbidEdit();

    taskWindow->show();
}

void Controller::processTask(QStringList lValues, QStringList rValues)
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
    if (lValues.size() == 0)
    {
        QMessageBox msgBox(taskWindow);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setText("Your task is empty");
        msgBox.exec();
    }
    else
        taskWindow->hide();
}
