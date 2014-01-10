#include "controller.h"

Controller::Controller(QWidget *parent) :
    QWidget(parent)
{

}

void Controller::initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, Model *_model)
{
    mainWindow = _mainWindow;
    connectionWindow = _connectionWindow;
    model = _model;

    mainWindow->setEnabledInputs(false);

    QObject::connect(this, SIGNAL(retrieveDrivers(QStringList&)), model, SLOT(drivers(QStringList&)));
    QObject::connect(this, SIGNAL(displayDrivers(QStringList&)), connectionWindow, SLOT(refreshDrivers(QStringList&)));

    QObject::connect(connectionWindow, SIGNAL(optionsSpecified()), this, SLOT(processConnectionOptions()));
    QObject::connect(this, SIGNAL(attemptToConnect(QHash<QString,QString>&)), model, SLOT(attemptToAddConnection(QHash<QString,QString>&)));

    QObject::connect(model, SIGNAL(connectionAttemptFinished(bool)), connectionWindow, SLOT(setHidden(bool)));
    QObject::connect(model, SIGNAL(connectionAttemptFinished(bool)), mainWindow, SLOT(setEnabledInputs(bool)));

    QObject::connect(model, SIGNAL(statusChanged(QString,int)), this, SIGNAL(statusChanged(QString,int)));
    QObject::connect(this, SIGNAL(statusChanged(QString,int)), mainWindow, SLOT(refreshStatus(QString,int)));
}

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

    connectionWindow->getOptions(options);
    emit attemptToConnect(options);
}

