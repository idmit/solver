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
#define SELECT_TYPES "SELECT name FROM TYPES"
#define SELECT_HISTORY "SELECT value, left_right, task_id FROM EQUATIONS WHERE task_id IN (SELECT id FROM TASKS WHERE type_id = :typeId)"
#define SELECT_METHODS "SELECT name FROM METHODS WHERE type_id = :typeId"
#define INSERT_TASK  "INSERT INTO Tasks (content, type_id) VALUES (:taskContent, :typeId)"

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
    int processedTaskTypeId() { return processedTask->typeId; }

signals:
    void statusChanged(QString status, int timeout);
    void connectionAttemptFinished(bool result);
    void askMeta(QStringList keys, QHash<QString, QString> *textMeta);
    void preventDataLoss(QStringList lValues, QStringList rValues);
    void alert (QString msg, int id);

public slots:
    /* Export db drivers */
    void retrieveDrivers(QStringList &drivers);
    /* Attempt to open a connection */
    void attemptToAddConnection(QHash<QString, QString> &options, bool &result);

    void retrieveTaskTypes(QStringList &taskTypes);
    void retrieveTaskHistory(int taskTypeId, QStringList &taskHistory);
    void retrieveSolutionMethods(int taskTypeId, QStringList &solutionMethods);
    void retrieveTaskFromHistory(int &taskId, int taskTypeId, int taskNumberInHistory, QStringList &lValues, QStringList &rValues);
    void retrieveTaskIdFromHistory(int &taskId, int taskTypeId, int taskNumberInHistory);
    void retrieveSolutionMethodFromList(int &solutionMethodId, int solutionMethodNumberInList);
    bool retrieveSolutionForProcessedTask(int solutionMethodId, QStringList *solution = 0);

    void regTask(int taskId, int taskTypeId, bool isNew);
    void makeTaskNew();

    bool solveTask(QStringList lValues, QStringList rValues, int solutionMethodId);

    void setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene);

    void removeSelectedTasks(QVector<int> selectedNumbers, int currentTypeId);
private:
    Task *processedTask;

    bool taskIsValid(QStringList lValues, QStringList rValues);
    void parseTask(QStringList lValues, QStringList rValues, Matrix &matrix, Vector &column);
    int saveTask(Matrix matrix, Vector column);
    void saveSolution(Vector result, int solutionMethodId, QHash<QString, double> meta);
    bool metaIsValid(QHash<QString, QString> textMeta, QHash<QString, double> &meta);
    double bisection(double a, double b, double precision);
};

#endif // MODEL_H
