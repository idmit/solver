#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QHash>

/* SQL QUERIES */
#define CONNECTION_NAME  "Solver"
#define CONNECTION_SUCC "Connection successful"
#define CONNECTION_ERR  "Error occured"

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
    void attemptToAddConnection(QHash<QString, QString> &options);
};

#endif // MODEL_H
