#include "model.h"
#include <QtSql>

Model::Model(QObject *parent) :
    QObject(parent)
{
}

void Model::drivers(QStringList &drivers)
{
    drivers = QSqlDatabase::drivers();
}

void Model::attemptToAddConnection(QHash<QString, QString> &options)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if (db.isValid())
    {
        db.close();
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }

    db = QSqlDatabase::addDatabase(options["driver"], QObject::tr(CONNECTION_NAME));
    bool result;

    db.setDatabaseName(options["dbname"]);
    db.setHostName(options["hostname"]);
    db.setPort(options["port"].toInt());

    result = db.open(options["username"], options["password"]);

    if (result)
    {
        emit statusChanged(CONNECTION_SUCC, 2000);
    }
    else
    {
        emit statusChanged(CONNECTION_ERR, 2000);
    }

    emit connectionAttemptFinished(result);
}
