#include "controller.h"
#include <QMessageBox>

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

    QObject::connect(mainWindow, SIGNAL(newTaskButtonClicked(int)), this, SLOT(showTaskWindow(int)));

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

void Controller::showTaskWindow(int taskNumberInHistory)
{
    taskWindow->show();
}
