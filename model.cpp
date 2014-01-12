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

    query.prepare(SELECT_HISRTORY);
    query.bindValue(":typeId", taskTypeId);
    query.exec();
    while (query.next())
    {
        taskHistory << query.value(0).toString();
    }
}
