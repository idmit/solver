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
#define INCOMPLETE_TASK_MSG "Your input is incomplete"
#define DELETE_WARNING "You are going to permanently delete some tasks. Are you sure?"
#define SAVE_SUCCESS_MSG "Selected tasks were successfully saved"
#define SOLUTION_GROUP_TEXT "Solution"
#define STATUS_DURATION 4000

class Controller : public QWidget
{
    Q_OBJECT
public:
    explicit Controller(QWidget *parent = 0);

signals:
    /*
     * Sent when status of some task changed, passing new status and timeout
     */
    void statusChanged(QString status, int timeout);

public slots:
    /*
    (IN) _mainWindow -- pointer to the main application window
    (IN) _connectionWindow -- pointer to the connection addition window
    (IN) _taskWindow -- pointer to the task processing window
    (IN) _model -- pointer to the model object 
     */
    void initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, TaskWindow *_taskWindow, Model *_model);

    /*
    (IN) msg -- string to be displayed as a warning pop-up
    (IN) parent -- pointer to window from which pop-up should appear
     */
    void alert(QString msg, QWidget *parent);

    void showConnectionWindow();
    void processConnectionOptions();

    /*
    (IN) connectionExists -- task types will be shown only if this is true
     */
    void showTaskTypes(bool connectionExists);
    /*
    (IN) taskTypeIndex -- list index of type for which history is requested 
     */
    void showTaskHistory(int taskTypeIndex);

    /*
    (IN) taskTypeId -- id of type for which methods are requested 
     */
    void showSolutionMethods(int taskTypeId);
    /*
    (IN) taskIndexInHistory -- index of task in the main history list 
     */
    void showTaskWindow(int taskIndexInHistory);

    void solveTask();
    /*
    (IN) lValues -- list of strings to be cleaned up from empty entries
    (IN) lValues -- list of strings to be cleaned up from empty entries
     */
    void removeRedundantData(QStringList &lValues, QStringList &rValues);
    /*
    (IN) keys -- list of names for values which should be provided by user
    (OUT) textMeta -- hash with provided answer for each key
     */
    void askMeta(QStringList keys, QHash<QString, QString> *textMeta);
    /*
    (IN) solutionMethodId -- id of a solution method with which task in process solution should exist
     */
    void showLastSolution();
    void showTaskSolutions();

    /*
    (IN) width -- width of graphics view
    (IN) height -- height of graphics view
    (IN) solution -- list of strings (one for each coordinate)
    (OUT) scene -- scene to be filled with solution visualization
     */
    void setUpScene(int width, int height, QGraphicsScene *scene);
    
    void proposeToSaveSession();
    void quitApp();

    /*
    (IN) lValues -- list of left sides of task equations
    (IN) rValues -- list of right sides of task equations
     */
    void createTask(QStringList lValues, QStringList rValues);
    void saveSessionItems();
    void deleteHistoryItems();
    
private:
    QDialog *createDialog(QString msg, QWidget *parent);

    MainWindow *mainWindow;
    ConnectionWindow *connectionWindow;
    TaskWindow *taskWindow;
    Model *model;
};

#endif // CONTROLLER_H
