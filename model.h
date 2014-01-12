#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QHash>

/* SQL QUERIES */
#define CONNECTION_NAME  "Solver"
#define SELECT_TYPES "SELECT name FROM TYPES"
#define SELECT_HISRTORY "SELECT content FROM TASKS WHERE type_id = :typeId"

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);

signals:
    void statusChanged(QString status, int timeout);
    void connectionAttemptFinished(bool result);

public slots:
    /* Export db drivers */
    void drivers(QStringList &drivers);
    /* Attempt to open a connection */
    void attemptToAddConnection(QHash<QString, QString> &options, bool &result);

    void taskTypes(QStringList &taskTypes);
    void taskHistory(int taskTypeId, QStringList &taskHistory);
};

#endif // MODEL_H
