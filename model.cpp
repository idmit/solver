#include "model.h"
#include <QtSql>

Model::Model(QObject *parent) :
    QObject(parent)
{
    processedTask = new Task(1);
}

/* CONNECTION CREATION BEGIN */

void Model::drivers(QStringList &drivers)
{
    drivers = QSqlDatabase::drivers();
}

void Model::attemptToAddConnection(QHash<QString, QString> &options, bool &result)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if (db.isValid())
    {
        db.close();
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }

    db = QSqlDatabase::addDatabase(options["driver"], QObject::tr(CONNECTION_NAME));

    db.setDatabaseName(options["dbname"]);
    db.setHostName(options["hostname"]);
    db.setPort(options["port"].toInt());

    result = db.open(options["username"], options["password"]);
}

/* CONNECTION CREATION END */

void Model::taskTypes(QStringList &taskTypes)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.exec(SELECT_TYPES);
    while (query.next())
    {
        taskTypes << query.value(0).toString();
    }
}

void Model::taskHistory(int taskTypeId, QStringList &taskHistory)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_HISTORY);
    query.bindValue(":typeId", taskTypeId);
    query.exec();

    int prevTaskId = 0, taskId = 1;
    double value = 0;
    bool leftOrRight = false, prevLeftOrRight = false;
    QString task;

    while (query.next())
    {
        prevTaskId = taskId;
        prevLeftOrRight = leftOrRight;

        value = query.value(0).toDouble();
        leftOrRight = query.value(1).toBool();
        taskId = query.value(2).toInt();

        if (taskId != prevTaskId)
        {
            taskHistory << task;
            task = "";
        }

        if (!task.isEmpty() && leftOrRight != prevLeftOrRight)
        {
            task += leftOrRight ? " =" : " ,";
        }

        if (!task.isEmpty())
        {
            task += " ";
        }
        task += QString::number(value);
    }

    taskHistory << task;

    if (taskHistory[0].isEmpty())
    {
        for (int i = 1; i < taskHistory.size(); ++i)
            taskHistory[i - 1] = taskHistory[i];
        taskHistory.removeAt(taskHistory.size() - 1);
    }
}

void Model::solutionMethods(int taskTypeId, QStringList &solutionMethods)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_METHODS);
    query.bindValue(":typeId", taskTypeId);
    query.exec();
    while (query.next())
    {
        solutionMethods << query.value(0).toString();
    }
}

void Model::taskFromHistory(int &expTaskId, int taskTypeId, int taskNumberInHistory, QStringList &lValues, QStringList &rValues)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_HISTORY);
    query.bindValue(":typeId", taskTypeId);
    query.exec();

    int prevTaskId = 0, taskId = 1, i = 0;
    QString value = "", row = "";
    bool leftOrRight = false, prevLeftOrRight = false;
    QStringList *values = &lValues;

    if (taskNumberInHistory == 1)
        query.next();
    else
        while (query.next())
        {
            prevTaskId = taskId;
            prevLeftOrRight = leftOrRight;
            taskId = query.value(2).toInt();

            if (i == 0)
            {
                prevTaskId = taskId; i = 1;
            }

            if (taskId != prevTaskId)
            {
                i++;
                if (i == taskNumberInHistory)
                {
                    break;
                }
            }
        }

    taskId = query.value(2).toInt();
    expTaskId = taskId;

    do
    {
        prevTaskId = taskId;
        prevLeftOrRight = leftOrRight;

        value = query.value(0).toString();
        leftOrRight = query.value(1).toBool();
        taskId = query.value(2).toInt();

        if (taskId != prevTaskId)
        {
            break;
        }

        if (leftOrRight != prevLeftOrRight)
        {
            values->append(row);
            row = "";
            values = leftOrRight ? &rValues : &lValues;
        }

        row += value + " ";
    } while (query.next());
    rValues.append(row);
}

void Model::regTask(int taskId, int taskTypeId, bool isNew)
{
    processedTask->id = taskId;
    processedTask->typeId = taskTypeId;
    processedTask->isNew = isNew;
}

void Model::makeTaskNew()
{
    processedTask->id = 0;
    processedTask->isNew = true;
}

void Model::solutionMethodFromList(int &solutionMethodId, int solutionMethodNumberInList)
{
    int i = 0;
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare("SELECT id FROM Methods WHERE type_id = :typeId");
    query.bindValue(":typeId", processedTask->typeId);
    query.exec();
    while (query.next() && (++i) != solutionMethodNumberInList);
    solutionMethodId = query.value(0).toInt();
}

bool Model::taskIsValid(QStringList lValues, QStringList rValues)
{
    int expectedDim = lValues.size();

    if (processedTask->typeId == SLAE_TYPE_ID)
    {
        for (int i = 0; i < expectedDim; ++i)
        {
           if (lValues[i].split(" ", QString::SkipEmptyParts).size() != expectedDim || rValues[i].split(" ", QString::SkipEmptyParts).size() != 1)
           {
               return false;
           }
        }
    }

    return true;
}

void Model::parseTask(QStringList lValues, QStringList rValues, Matrix &matrix, Vector &column)
{
    int dim = lValues.size();
    QVector<double> row(0);

    for (int i = 0; i < dim; ++i)
    {
        row.clear();
        foreach (const QString &number, lValues[i].split(" ", QString::SkipEmptyParts))
        {
            row << number.toDouble();
        }
        matrix[i] = row;

        column[i] = rValues[i].split(" ", QString::SkipEmptyParts)[0].toDouble();
    }
}

int Model::saveTask(Matrix matrix, Vector column)
{
    int newTaskId = 0;
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare("INSERT INTO Tasks (type_id) VALUES (:typeId)");
    query.bindValue(":typeId", processedTask->typeId);
    query.exec();

    newTaskId = query.lastInsertId().toInt();

    for (int i = 0; i < matrix.dim(); ++i)
    {
        for (int j = 0; j < matrix.dim(); ++j)
        {
            query.prepare("INSERT INTO Equations (value, left_right, task_id) VALUES (:value, :leftRight, :taskId)");
            query.bindValue(":taskId", newTaskId);
            query.bindValue(":value", matrix[i][j]);
            query.bindValue(":leftRight", 0);
            query.exec();
        }
        query.prepare("INSERT INTO Equations (value, left_right, task_id) VALUES (:value, :leftRight, :taskId)");
        query.bindValue(":taskId", newTaskId);
        query.bindValue(":value", column[i]);
        query.bindValue(":leftRight", 1);
        query.exec();
    }

    return newTaskId;
}

void Model::solution(QString &solution, int solutionMethodId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare("SELECT value FROM Solutions WHERE task_id = :taskId AND method_id = :methodId");
    query.bindValue(":taskId", processedTask->id);
    query.bindValue(":methodId", solutionMethodId);
    query.exec();

    solution = "";
    while (query.next())
    {
        solution += query.value(0).toString() + "\n";
    }
}

void Model::saveSolution(Vector result, int solutionMethodId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    for (int i = 0; i < result.dim(); ++i)
    {
        query.prepare("INSERT INTO Solutions (task_id, method_id, value) VALUES (:taskId, :methodId, :value)");
        query.bindValue(":taskId", processedTask->id);
        query.bindValue(":value", result[i]);
        query.bindValue(":methodId", solutionMethodId);
        query.exec();
    }
}

void Model::attemptToFindSolution(int solutionMethodId, bool &found)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare("SELECT id FROM SOLUTIONS WHERE task_id = :taskId AND method_id = :methodId");
    query.bindValue(":taskId", processedTask->id);
    query.bindValue(":methodId", solutionMethodId);
    query.exec();

    found = false;

    while (query.next() && (found = true));
}

void Model::solveTask(QStringList lValues, QStringList rValues, int solutionMethodId)
{
    int dim = lValues.size();
    bool found = false;
    processedTask->matrix = Matrix(dim);
    processedTask->vector = Vector(dim);

    if (!processedTask->isNew)
        attemptToFindSolution(solutionMethodId, found);

    if (found)
        return;

    if (!taskIsValid(lValues, rValues))
    {
        emit statusChanged("Wrong!", 3000);
        return;
    }

    Vector column(dim), result(dim);
    Matrix matrix(dim);

    parseTask(lValues, rValues, matrix, column);

    result = matrix.reflection(column);

    if (processedTask->isNew)
        processedTask->id = saveTask(matrix, column);

    saveSolution(result, solutionMethodId);
}
