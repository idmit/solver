#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include "matrix.h"

/* Type IDS in DB */
#define LE_TYPE_ID   1
#define SLAE_TYPE_ID 2

/* SQL QUERIES */
#define CONNECTION_NAME  "Solver"
#define SELECT_TYPES "SELECT name FROM TYPES"
#define SELECT_HISTORY "SELECT value, left_right, task_id FROM EQUATIONS WHERE task_id IN (SELECT id FROM TASKS WHERE type_id = :typeId)"
#define SELECT_METHODS "SELECT name FROM METHODS WHERE type_id = :typeId"
#define INSERT_TASK  "INSERT INTO Tasks (content, type_id) VALUES (:taskContent, :typeId)"

struct Task
{
    Task(int dim): id(0), typeId(0), isNew(false), matrix(dim), vector(dim) {}
    int id;
    int typeId;
    bool isNew;
    Matrix matrix;
    Vector vector;
};

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);
    int processedTaskTypeId() { return processedTask->typeId; }

signals:
    void statusChanged(QString status, int timeout);
    void connectionAttemptFinished(bool result);

public slots:
    /* Export db drivers */
    void retrieveDrivers(QStringList &drivers);
    /* Attempt to open a connection */
    void attemptToAddConnection(QHash<QString, QString> &options, bool &result);

    void retrieveTaskTypes(QStringList &taskTypes);
    void retrieveTaskHistory(int taskTypeId, QStringList &taskHistory);
    void retrieveSolutionMethods(int taskTypeId, QStringList &solutionMethods);
    void retrieveTaskFromHistory(int &taskId, int taskTypeId, int taskNumberInHistory, QStringList &lValues, QStringList &rValues);
    void retrieveSolutionMethodFromList(int &solutionMethodId, int solutionMethodNumberInList);
    void retrieveSolutionForProcessedTask(QString &solution, int solutionMethodId);

    void regTask(int taskId, int taskTypeId, bool isNew);
    void makeTaskNew();

    void solveTask(QStringList lValues, QStringList rValues, int solutionMethodId);

private:
    Task *processedTask;

    bool taskIsValid(QStringList lValues, QStringList rValues);
    void parseTask(QStringList lValues, QStringList rValues, Matrix &matrix, Vector &column);
    int saveTask(Matrix matrix, Vector column);
    void saveSolution(Vector result, int solutionMethodId);
    void attemptToFindSolution(int solutionMethodId, bool &found);
};

#endif // MODEL_H
