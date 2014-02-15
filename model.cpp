#include "model.h"
#include <QtSql>

Model::Model(QObject *parent) :
    QObject(parent)
{
    processedTask = new Task();
}

/* CONNECTION CREATION BEGIN */

void Model::retrieveDrivers(QStringList &drivers)
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

void Model::retrieveTaskTypes(QStringList &taskTypes)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.exec(SELECT_TYPES);
    while (query.next())
    {
        taskTypes << query.value(0).toString();
    }
}

void Model::retrieveTaskHistory(int taskTypeId, QStringList &taskHistory)
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

void Model::retrieveSolutionMethods(int taskTypeId, QStringList &solutionMethods)
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

void Model::retrieveTaskFromHistory(int &expTaskId, int taskTypeId, int taskNumberInHistory, QStringList &lValues, QStringList &rValues)
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

void Model::retrieveSolutionMethodFromList(int &solutionMethodId, int solutionMethodNumberInList)
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

    switch (processedTask->typeId)
    {
    case LE_TYPE_ID:
    case SLAE_TYPE_ID:
        for (int i = 0; i < expectedDim; ++i)
        {
           if (lValues[i].split(" ", QString::SkipEmptyParts).size() != expectedDim || rValues[i].split(" ", QString::SkipEmptyParts).size() != 1)
           {
               return false;
           }
        }
        break;
    default:
        break;
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

void Model::retrieveSolutionForProcessedTask(QString &solution, int solutionMethodId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare("SELECT id, value FROM Solutions WHERE task_id = :taskId AND method_id = :methodId");
    query.bindValue(":taskId", processedTask->id);
    query.bindValue(":methodId", solutionMethodId);
    query.exec();

    solution = "";

    while (query.next())
    {
        int solutionId = query.value(0).toInt();

        QSqlQuery q(db);

        q.prepare("SELECT name, value FROM Meta WHERE solution_id = :solutionId");
        q.bindValue(":solutionId", solutionId);
        q.exec();

        int i = 0;
        bool found = true;
        while (q.next())
        {
            if (processedTask->meta.values()[i] != q.value(1).toDouble())
            {
                found = false;
            }
            i++;
        }

        if (found)
        {
            solution += query.value(1).toString() + "\n";
        }
    }
}

void Model::saveSolution(Vector result, int solutionMethodId, QHash<QString, double> meta)
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

    int solutionId = query.lastInsertId().toInt();

    for (int i = 0; i < meta.values().size(); ++i)
    {
        query.prepare("INSERT INTO Meta (solution_id, name, value) VALUES (:solutionId, :name, :value)");
        query.bindValue(":solutionId", solutionId);
        query.bindValue(":name", meta.keys()[i]);
        query.bindValue(":value", meta.values()[i]);
        query.exec();
    }
}

bool Model::attemptToFindSolution(int solutionMethodId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare("SELECT id FROM SOLUTIONS WHERE task_id = :taskId AND method_id = :methodId");
    query.bindValue(":taskId", processedTask->id);
    query.bindValue(":methodId", solutionMethodId);
    query.exec();

    while (query.next())
    {
        int solutionId = query.value(0).toInt();

        QSqlQuery q(db);

        q.prepare("SELECT name, value FROM Meta WHERE solution_id = :solutionId");
        q.bindValue(":solutionId", solutionId);
        q.exec();

        int i = 0;
        bool found = true;
        while (q.next())
        {
            if (processedTask->meta.values()[i] != q.value(1).toDouble())
            {
                found = false;
            }
            i++;
        }

        if (found)
        {
            return true;
        }
    }

    return false;
}

bool Model::metaIsValid(QHash<QString, QString> textMeta, QHash<QString, double> &meta)
{
    for (int i = 0; i < textMeta.size(); ++i)
    {
        if (textMeta.values()[i].split(" ", QString::SkipEmptyParts).size() != 1)
        {
            meta.clear();
            return false;
        }
        meta[textMeta.keys()[i]] = textMeta.values()[i].split(" ", QString::SkipEmptyParts)[i].toDouble();
    }
    return true;
}

double Model::bisection(double a, double b, double precision)
{
    double lb = -50,
            rb = 50,
            m = 0;

    while ((a * lb - b) * (a * rb - b) > 0)
    {
        lb -= 10;
        rb += 10;
    }

    while (rb - lb > precision)
    {
        m = (lb + rb) / 2;
        if ((a * lb - b) * (a * m - b) < 0)
            rb = m;
        else
            lb = m;
    }

    return (lb + rb) / 2;
}

bool Model::solveTask(QStringList lValues, QStringList rValues, int solutionMethodId)
{
    int dim = lValues.size();
    QHash<QString, QString> textMeta;
    QHash<QString, double> meta;

    if (!taskIsValid(lValues, rValues))
    {
        emit alert("Your input is incomplete.", 3000);
        return false;
    }

    Vector column(dim), result(dim);
    Matrix matrix(dim);

    parseTask(lValues, rValues, matrix, column);

    switch (solutionMethodId)
    {
    case NATIVE_METHOD_ID:
    case REFLECTION_METHOD_ID:
        result = matrix.reflection(column);
        break;
    case BISECTION_METHOD_ID:
    {
        QStringList keys;
        keys << "Precision of bisection";
        emit askMeta(keys, &textMeta);
        if (textMeta.isEmpty())
            return false;
        if (!metaIsValid(textMeta, meta))
        {
            emit alert("You must fill all the fields to use chosen method", 3);
            return false;
        }
        processedTask->meta = meta;
        result[0] = bisection(matrix[0][0], column[0], meta["Precision of bisection"]);
        break;
    }
    default:
        break;
    }

    if (!processedTask->isNew)
        if (attemptToFindSolution(solutionMethodId))
            return true;

    if (processedTask->isNew)
        processedTask->id = saveTask(matrix, column);

    saveSolution(result, solutionMethodId, meta);
    return true;
}
