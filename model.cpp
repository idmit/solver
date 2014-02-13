#include "model.h"
#include <QtSql>

Model::Model(QObject *parent) :
    QObject(parent)
{
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

    int prevTaskId = 0, taskId = 1, i = 1;
    QString value = "";
    bool leftOrRight = false, prevLeftOrRight = false;
    QStringList *values = &lValues;

    while (query.next() && i != taskNumberInHistory)
    {
        prevTaskId = taskId;
        prevLeftOrRight = leftOrRight;
        taskId = query.value(2).toInt();

        if (taskId != prevTaskId)
        {
            i++;
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
            values = leftOrRight ? &rValues : &lValues;
        }

        values->append(value + " ");
    } while (query.next());
}
