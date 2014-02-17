#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include "mainwindow.h"
#include "connectionwindow.h"
#include "taskwindow.h"
#include "model.h"

#define CONNECTION_SUCC "Connection successful"
#define CONNECTION_ERR  "Error occured"

class Controller : public QWidget
{
    Q_OBJECT
public:
    explicit Controller(QWidget *parent = 0);

signals:
    /* Inform the user about some changes */
    void statusChanged(QString status, int timeout);

    /* Connection related */
    void retrieveDrivers(QStringList &drivers);
    void displayDrivers(QStringList &drivers);
    void attemptToConnect(QHash<QString, QString> &options, bool &result);

    /* Load and show task types */
    void retrieveTaskTypes(QStringList &taskTypes);
    void displayTaskTypes(QStringList &taskTypes);

    /* Load and show history of tasks */
    void retrieveTaskHistory(int taskTypeId, QStringList &taskHistory);
    void displayTaskHistory(QStringList &taskHistory);

    /* Load and show task solution methods */
    void retrieveSolutionMethods(int taskTypeId, QStringList &solutionMethods);
    void displaySolutionMethods(QStringList &solutionMethods);

    /* Register task with parameters as processing */
    void regTask(int taskId, int taskTypeId, bool isNewTask);

    /* Load and show task chosen from history */
    void retrieveTaskFromHistory(int &taskId, int taskTypeId, int taskNumberInHistory, QStringList &lValues, QStringList &rValues);
    void displayTaskFromHistory(QStringList lValues, QStringList rValues);

    void retrieveSolutionMethodId(int &solutionMethodId, int solutionMethodNumberInList);
    void solveTask(QStringList lValues, QStringList rValues, int solutionMethodId);

    void retrieveSolution(QString &solution, int solutionMethodId);
    void displaySolution(QString solution);

public slots:
    void initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, TaskWindow *_taskWindow, Model *_model);

    void showConnectionWindow();
    void processConnectionOptions();

    void showTaskTypes(bool connectionExists);
    void showTaskHistory(int taskTypeIndex);

    void showSolutionMethods(int taskTypeId);
    void showTaskWindow(int taskIndexInHistory);

    void showSolution(int solutionMethodId);

    void processTask(QStringList lValues, QStringList rValues);

    void removeRedundantData(QStringList &lValues, QStringList &rValues);
    void askMeta(QStringList keys, QHash<QString, QString> *textMeta);

    void alert(QString msg, int id);
    void setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene);

    void deleteHistoryItem();

private:
    MainWindow *mainWindow;
    ConnectionWindow *connectionWindow;
    TaskWindow *taskWindow;
    Model *model;
};

#endif // CONTROLLER_H
