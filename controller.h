#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include "mainwindow.h"
#include "connectionwindow.h"
#include "taskwindow.h"
#include "model.h"

#define CONNECTION_SUCC "Connection successful"
#define CONNECTION_ERR  "Error occured"
#define OK_TEXT "OK"
#define CANCEL_TEXT "Cancel"
#define ASK_META_MSG "Provide some additional information:"
#define INVALID_META_MSG "You must fill all the fields to use chosen method"
#define EMPTY_TASK_MSG "Your task is empty"
#define INCOMPLETE_TASK_MSG "Your input is incomplete."
#define DELETE_WARNING "You are going to permanently delete some tasks. Are you sure?"
#define IMAGE_BUTTON_TEXT "Save as Image"
#define SOLUTION_GROUP_TEXT "Solution"

class Controller : public QWidget
{
    Q_OBJECT
public:
    explicit Controller(QWidget *parent = 0);

signals:
    /* Inform the user about some changes */
    void statusChanged(QString status, int timeout);

public slots:
    void initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, TaskWindow *_taskWindow, Model *_model);

    void alert(QString msg, int id);

    void showConnectionWindow();
    void processConnectionOptions();

    void showTaskTypes(bool connectionExists);
    void showTaskHistory(int taskTypeIndex);

    void showSolutionMethods(int taskTypeId);
    void showTaskWindow(int taskIndexInHistory);

    void processTask(QStringList lValues, QStringList rValues);
    void removeRedundantData(QStringList &lValues, QStringList &rValues);
    void askMeta(QStringList keys, QHash<QString, QString> *textMeta);
    void showSolution(int solutionMethodId);

    void setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene);

    void deleteHistoryItem();
private:
    MainWindow *mainWindow;
    ConnectionWindow *connectionWindow;
    TaskWindow *taskWindow;
    Model *model;
};

#endif // CONTROLLER_H
