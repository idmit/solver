#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QHash>
#include <QGraphicsScene>
#include <QStringList>
#include "matrix.h"

/* Type IDS in DB */
#define LE_TYPE_ID   1
#define SLAE_TYPE_ID 2

/* Methods ID in DB */
#define NATIVE_METHOD_ID    1
#define BISECTION_METHOD_ID 2
#define REFLECTION_METHOD_ID    3
#define SEIDEL_METHOD_ID 4

/* SQL QUERIES */
#define CONNECTION_NAME  "Solver"
#define SELECT_TYPES_NAMES "SELECT name FROM TYPES"
#define SELECT_TASKS_IDS "SELECT id FROM TASKS WHERE type_id = :typeId"
#define SELECT_EQS_ALL "SELECT value, left_right, task_id FROM EQUATIONS WHERE task_id IN (SELECT id FROM TASKS WHERE type_id = :typeId)"
#define SELECT_EQS "SELECT value, left_right FROM EQUATIONS WHERE task_id = :taskId"
#define SELECT_METHODS_NAMES "SELECT name FROM METHODS WHERE type_id = :typeId"
#define SELECT_METHODS_IDS "SELECT id FROM Methods WHERE type_id = :typeId"
#define INSERT_TASK "INSERT INTO Tasks (type_id) VALUES (:typeId)"
#define INSERT_EQ "INSERT INTO Equations (value, left_right, task_id) VALUES (:value, :leftRight, :taskId)"
#define SELECT_SOLUTIONS "SELECT id, value FROM Solutions WHERE task_id = :taskId AND method_id = :methodId"
#define SELECT_META "SELECT name, value FROM Meta WHERE solution_id = :solutionId"
#define INSERT_SOLUTION "INSERT INTO Solutions (task_id, method_id, value) VALUES (:taskId, :methodId, :value)"
#define INSERT_META "INSERT INTO Meta (solution_id, name, value) VALUES (:solutionId, :name, :value)"
#define DELETE_TASK "DELETE FROM Tasks WHERE id = :taskId"

enum InputCompleteness
{
    INPUT_COMPLETE,
    INPUT_INCOMPLETE_TASK,
    INPUT_INVALID_META
};

struct Task
{
    Task(): id(0), typeId(0), isNew(false), meta() {}
    int id;
    int typeId;
    bool isNew;
    QHash<QString, double> meta;
};

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);
    int taskInProcessTypeId() { return taskInProcess->typeId; }

signals:
    void connectionAttemptFinished(bool result);
    void getMeta(QStringList keys, QHash<QString, QString> *textMeta);

public slots:
    /*
    (OUT) drivers -- list of available DB drivers
     */
    void retrieveDrivers(QStringList &drivers);
    /*
    (IN) options -- hash named connection options:
    **** driver, dbname, username, password, hostname
    (OUT) result -- success of connection opening
     */
    void attemptToAddConnection(QHash<QString, QString> &options, bool &result);

    /*
    (OUT) taskTypes -- list of available types of tasks
     */
    void retrieveTaskTypes(QStringList &taskTypes);
    /*
    (IN) taskTypeId -- id of type for which history is requested 
    (OUT) taskHistory -- list of strings (one for each task with comma separated equations in it)
     */
    void retrieveTaskHistory(int taskTypeId, QStringList &taskHistory);
    /*
    (IN) taskTypeId -- id of type for which available solution methods are requested
    (OUT) solutionMethods -- list of available solution methods
     */
    void retrieveSolutionMethods(int taskTypeId, QStringList &solutionMethods);
    /*
    (IN) taskTypeId -- id of type from which history task description is requested
    (IN) taskNumberInHistory -- number of task in history of that type
    (OUT) taskId -- id of task
    (OUT) lValues -- list of left sides of task's equations
    (OUT) rValues -- list of right sides of task's equations
     */
    void retrieveTaskFromHistory(int &taskId, int taskTypeId, int taskNumberInHistory, QStringList *lValues = 0, QStringList *rValues = 0);
    /*
    (IN) solutionMethodNumberInList -- number of solution method in list for task in process
    (OUT) solutionMethodId -- id of solution method
     */
    void retrieveSolutionMethodFromList(int &solutionMethodId, int solutionMethodNumberInList);
    /*
    (IN) solutionMethodId -- id of solution method
    (OUT) solution -- list of strings (one for each coordinate)
    (OUT) true if solution exists
     */
    bool retrieveSolutionForTaskInProcess(int solutionMethodId, QStringList *solution = 0);

    /*
    (IN) taskId -- id to be set as in process
    (IN) taskTypeId -- id of type to be set as in process
    (IN) isNew -- true sets task in process as new
     */
    void setTaskInProcess(int taskId, int taskTypeId, bool isNew);
    void setTaskInProcessAsNew();

    /*
    (IN) lValues -- list of left sides of task equations
    (IN) rValues --list of right sides of task equations
    (IN) solutionMethodId -- id of solution method to solve given task
    (OUT) INPUT_COMPLETE if task formulation is correct
     */
    InputCompleteness solveTask(QStringList lValues, QStringList rValues, int solutionMethodId);

    /*
    (IN) width -- width of graphics view
    (IN) height -- height of graphics view
    (IN) solution -- list of strings (one for each coordinate)
    (OUT) scene -- scene to be filled with solution visualization
     */
    void setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene);

    /*
    
    (IN) taskId -- id of type from which history task deletion is requested
    (IN) numbersInHistory -- numbers of task in history of that type
     */
    void eraseSelectedTasks(QVector<int> numbersInHistory, int typeId);
private:
    Task *taskInProcess;

    /*
    (IN) lValues -- list of left sides of task equations
    (IN) rValues -- list of right sides of task equations
    (OUT) true if task formulation is valid
     */
    bool taskIsValid(QStringList lValues, QStringList rValues);
    /*
    (IN) lValues -- list of left sides of task equations
    (IN) rValues -- list of right sides of task equations
    (OUT) matrix -- matrix containing that task's left sides
    (OUT) vector -- vector containing that task's right sides
     */
    void parseTask(QStringList lValues, QStringList rValues, Matrix &matrix, Vector &column);
    /*
    (IN) matrix -- matrix containing that task's left sides
    (IN) vector -- vector containing that task's right sides
    (OUT) id of saved task
     */
    int saveTask(Matrix matrix, Vector column);
    /*
    (IN) result -- value associated with task in process
    (IN) solutionMethodId -- id of solution method with which that value was received
    (IN) meta -- hash with additional values
     */
    void saveSolution(Vector result, int solutionMethodId, QHash<QString, double> meta);
    /*
    (IN) textMeta -- hash of strings
    (OUT) meta -- hash of values parsed out from textMeta
    (OUT) true if meta is valid and complete
     */
    bool metaIsValid(QHash<QString, QString> textMeta, QHash<QString, double> &meta);
    /*
    (IN) a, b -- coefficients as in a*x = b
    (IN) precision -- precision of found root
    (OUT) found root 
     */
    double bisection(double a, double b, double precision);
};

#endif // MODEL_H
