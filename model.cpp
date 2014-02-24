#include "model.h"
#include <QtSql>

#define ABS(x) (x > 0 ? x : -1 * x)

Model::Model(QObject *parent) :
    QObject(parent)
{
}

void Model::retrieveDrivers(QStringList &drivers)
{
    drivers = QSqlDatabase::drivers();
}

void Model::attemptToAddConnection(QHash<QString, QString> &options, bool &result)
{
    QSqlDatabase::removeDatabase(CONNECTION_NAME);

    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);

    db = QSqlDatabase::addDatabase(options["driver"], QObject::tr(CONNECTION_NAME));

    db.setDatabaseName(options["dbname"]);
    db.setHostName(options["hostname"]);
    db.setPort(options["port"].toInt());

    result = db.open(options["username"], options["password"]);
}

void Model::retrieveTaskTypes(QStringList &taskTypes)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.exec(SELECT_TYPES_NAMES);
    while (query.next())
    {
        taskTypes << query.value(0).toString();
    }
}

void Model::retrieveHistoryByType(int taskTypeId, QStringList &taskHistory)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_EQS_ALL);
    query.bindValue(":typeId", taskTypeId);
    query.exec();

    if (!query.next())
        return;

    int taskId = query.value(2).toInt();
    bool leftOrRight = false;
    QString blackCircle = QString::fromUtf8("\u25CF "), taskStringWithCircle, task;

    do
    {
        int prevTaskId = taskId;
        double value = 0;
        bool prevLeftOrRight = leftOrRight;

        value = query.value(0).toDouble();
        leftOrRight = query.value(1).toBool();
        taskId = query.value(2).toInt();

        if (taskId != prevTaskId)
        {
            taskStringWithCircle = blackCircle + task;
            taskHistory << taskStringWithCircle;
            task = "";
        }

        if (!task.isEmpty() && leftOrRight != prevLeftOrRight)
        {
            task += leftOrRight ? " =" : ",";
        }

        if (!task.isEmpty())
        {
            task += " ";
        }
        task += QString::number(value);
    } while (query.next());

    taskStringWithCircle = blackCircle + task;
    taskHistory << taskStringWithCircle;
}

void Model::retrieveSessionByType(int taskTypeId, QStringList &taskSession)
{
    Task task;
    Matrix matrix;
    Vector vector;
    QString emptyCircle = QString::fromUtf8("\u25CB "), blackCircle = QString::fromUtf8("\u25CF ");

    for (int i = 0; i < tasksInSession.size(); ++i)
    {
        task = tasksInSession[i];
        if (task.typeId != taskTypeId)
            continue;
        matrix = task.matrix;
        vector = task.vector;
        QString taskString = (task.idInDB == 0) ? emptyCircle : blackCircle;
        for (int k = 0; k < vector.dim(); ++k)
        {
            for (int m = 0; m < vector.dim(); ++m)
            {
                taskString += QString::number(matrix[k][m]) + " ";
            }
            taskString += "= " + QString::number(vector[k]);
            if (k != vector.dim() - 1) {taskString += ", "; }
        }
        taskSession << taskString;
    }
}

void Model::retrieveSolutionMethods(int taskTypeId, QStringList &solutionMethods)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_METHODS_NAMES);
    query.bindValue(":typeId", taskTypeId);
    query.exec();
    while (query.next())
    {
        solutionMethods << query.value(0).toString();
    }
}

void Model::retrieveTaskFromHistory(int &taskId, int taskTypeId, int taskNumberInHistory, QStringList *lValues, QStringList *rValues)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    int i = 0;
    QString value = "", row = "";
    bool leftOrRight = false;
    QStringList *values = lValues;

    query.prepare(SELECT_TASKS_IDS);
    query.bindValue(":typeId", taskTypeId);
    query.exec();

    while (query.next())
    {
        i += 1;
        if (i == taskNumberInHistory)
            break;
    }

    taskId = query.value(0).toInt();

    if (!lValues)
        return;

    query.prepare(SELECT_EQS);
    query.bindValue(":taskId", taskId);
    query.exec();

    while (query.next())
    {
        bool prevLeftOrRight = leftOrRight;

        value = query.value(0).toString();
        leftOrRight = query.value(1).toBool();

        if (leftOrRight != prevLeftOrRight)
        {
            values->append(row);
            row = "";
            values = leftOrRight ? rValues : lValues;
        }

        row += value + " ";
    }
    values->append(row);
}

void Model::retrieveTaskFromSession(int taskTypeId, int taskNumberInHistory, QStringList *lValues, QStringList *rValues)
{
    Task task;
    QVector<Task> tasks;
    QVector<int> indexes;
    Matrix matrix;
    Vector vector;

    for (int i = 0; i < tasksInSession.size(); ++i)
    {
        if (tasksInSession[i].typeId == taskTypeId)
        {
            tasks << tasksInSession[i];
            indexes << i;
        }
    }
    sessionIndexOfTaskInFocus = indexes[taskNumberInHistory - 1];
    task = tasks[taskNumberInHistory - 1];
    matrix = task.matrix;
    vector = task.vector;

    for (int k = 0; k < vector.dim(); ++k)
    {
        QString row = "";
        for (int m = 0; m < vector.dim(); ++m)
        {
            row += QString::number(matrix[k][m]) + " ";
        }
        lValues->append(row);
        rValues->append(QString::number(vector[k]));
    }
}

void Model::retrieveSolutionMethodFromList(int &solutionMethodId, int solutionMethodNumberInList)
{
    int i = 0;
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_METHODS_IDS);
    query.bindValue(":typeId", tasksInSession[sessionIndexOfTaskInFocus].typeId);
    query.exec();
    while (query.next() && (++i) != solutionMethodNumberInList);
    solutionMethodId = query.value(0).toInt();
}

bool Model::taskIsValid(QStringList lValues, QStringList rValues, int taskTypeId)
{
    int expectedDim = lValues.size();

    switch (taskTypeId)
    {
    case LE_TYPE_ID:
    case SLAE_TYPE_ID:
        for (int i = 0; i < expectedDim; ++i)
        {
            QStringList row = lValues[i].split(" ", QString::SkipEmptyParts);
            QStringList rightSide = rValues[i].split(" ", QString::SkipEmptyParts);
            bool converts = true;

            if (row.size() != expectedDim || rightSide.size() != 1)
            {
                return false;
            }

            for (int k = 0; k < row.size(); ++k)
            {
                row[k].toDouble(&converts);
                if (!converts) return false;
            }
            rightSide[0].toDouble(&converts);
            if (!converts) return false;
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

    query.prepare(INSERT_TASK);
    query.bindValue(":typeId", 0);
    query.exec();

    newTaskId = query.lastInsertId().toInt();

    for (int i = 0; i < matrix.dim(); ++i)
    {
        for (int j = 0; j < matrix.dim(); ++j)
        {
            query.prepare(INSERT_EQ);
            query.bindValue(":taskId", newTaskId);
            query.bindValue(":value", matrix[i][j]);
            query.bindValue(":leftRight", 0);
            query.exec();
        }
        query.prepare(INSERT_EQ);
        query.bindValue(":taskId", newTaskId);
        query.bindValue(":value", column[i]);
        query.bindValue(":leftRight", 1);
        query.exec();
    }

    return newTaskId;
}

int Model::saveTaskFromSession(int k)
{
    int newTaskId = 0;
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(INSERT_TASK);
    query.bindValue(":typeId", tasksInSession[k].typeId);
    query.exec();

    newTaskId = query.lastInsertId().toInt();

    for (int i = 0; i < tasksInSession[k].matrix.dim(); ++i)
    {
        for (int j = 0; j <  tasksInSession[k].matrix.dim(); ++j)
        {
            query.prepare(INSERT_EQ);
            query.bindValue(":taskId", newTaskId);
            query.bindValue(":value", tasksInSession[k].matrix[i][j]);
            query.bindValue(":leftRight", 0);
            query.exec();
        }
        query.prepare(INSERT_EQ);
        query.bindValue(":taskId", newTaskId);
        query.bindValue(":value", tasksInSession[k].vector[i]);
        query.bindValue(":leftRight", 1);
        query.exec();
    }

    return newTaskId;
}

bool Model::retrieveSolution(int taskIdInDB, int solutionMethodId, QHash<QString, double> meta, QStringList *solution)
{
    bool solutionExists = false;
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(SELECT_SOLUTIONS);
    query.bindValue(":taskId", taskIdInDB);
    query.bindValue(":methodId", solutionMethodId);
    query.exec();

    if (solution) solution->clear();

    while (query.next())
    {
        int solutionId = query.value(0).toInt();

        QSqlQuery q(db);

        q.prepare(SELECT_META);
        q.bindValue(":solutionId", solutionId);
        q.exec();

        int i = 0;
        bool found = true;
        while (q.next())
        {
            if (meta.values()[i] != q.value(1).toDouble())
            {
                found = false;
            }
            i++;
        }

        if (found)
        {
            solutionExists = true;
            if (!solution) return solutionExists;
            solution->append(query.value(1).toString());
        }
    }
    return solutionExists;
}

void Model::retrieveSessionSolutionValues(QStringList &solutionValues)
{
    QVector<Solution> solutions = tasksInSession[sessionIndexOfTaskInFocus].solutions;
    Solution solution = solutions.last();
    foreach (double value, solution.values)
    {
        solutionValues << QString::number(value);
    }
}

void Model::saveSolution(Vector result, int solutionMethodId, QHash<QString, double> meta)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    for (int i = 0; i < result.dim(); ++i)
    {
        query.prepare(INSERT_SOLUTION);
        query.bindValue(":taskId", 0);
        query.bindValue(":value", result[i]);
        query.bindValue(":methodId", solutionMethodId);
        query.exec();
    }

    int solutionId = query.lastInsertId().toInt();

    for (int i = 0; i < meta.values().size(); ++i)
    {
        query.prepare(INSERT_META);
        query.bindValue(":solutionId", solutionId);
        query.bindValue(":name", meta.keys()[i]);
        query.bindValue(":value", meta.values()[i]);
        query.exec();
    }
}

bool Model::metaIsValid(QHash<QString, QString> textMeta, QHash<QString, double> &meta)
{
    for (int i = 0; i < textMeta.size(); ++i)
    {
        QStringList textValues = textMeta.values()[i].split(" ", QString::SkipEmptyParts);
        if (textValues.size() != 1)
        {
            meta.clear();
            return false;
        }

        bool converts = false;
        textValues[0].toDouble(&converts);
        if (!converts) { meta.clear(); return false; }

        meta[textMeta.keys()[i]] = textValues[0].toDouble();
    }
    return true;
}

InputCompleteness Model::solveTask(int solutionMethodId)
{
    QHash<QString, QString> textMeta;
    QHash<QString, double> meta;
    QStringList solutionAsList;

    Vector column(0), result(tasksInSession[sessionIndexOfTaskInFocus].vector.dim());
    Matrix matrix(0);
    Solution solution;

    matrix = tasksInSession[sessionIndexOfTaskInFocus].matrix;
    column = tasksInSession[sessionIndexOfTaskInFocus].vector;

    solution.id = 0; solution.methodId = solutionMethodId; solution.isSaved = false;

    switch (solutionMethodId)
    {
    case NATIVE_METHOD_ID:
        break;
    case REFLECTION_METHOD_ID:
        break;
    case BISECTION_METHOD_ID:
    {
        if (matrix[0][0] == 0)
        {
            return INPUT_INCOMPLETE_TASK;
        }
        QStringList keys;
        keys << "Precision of bisection";
        emit getMeta(keys, &textMeta);
        if (!metaIsValid(textMeta, meta))
        {
            return INPUT_INVALID_META;
        }
        solution.meta = meta;
    }
        break;
    default:
        break;
    }

    if (tasksInSession[sessionIndexOfTaskInFocus].idInDB != 0)
        if (retrieveSolution(tasksInSession[sessionIndexOfTaskInFocus].idInDB, solutionMethodId, meta, &solutionAsList))
        {
            for (int k = 0; k < solutionAsList.size(); ++k)
            {
                result[k] = solutionAsList[k].toDouble();
            }
            solution.values = result;
            solution.isSaved = true;
            tasksInSession[sessionIndexOfTaskInFocus].solutions << solution;
            return INPUT_COMPLETE;
        }

    switch (solutionMethodId)
    {
    case NATIVE_METHOD_ID:
        if (matrix[0][0] == 0)
        {
            return INPUT_INCOMPLETE_TASK;
        }
    case REFLECTION_METHOD_ID:
        result = matrix.reflection(column);
        break;
    case BISECTION_METHOD_ID:
    {
        result[0] = bisection(matrix[0][0], column[0], meta["Precision of bisection"]);
        break;
    }
    default:
        break;
    }

    solution.values = result;
    tasksInSession[sessionIndexOfTaskInFocus].solutions << solution;
    return INPUT_COMPLETE;
}

static void addPointAt(QGraphicsScene *scene, double x, double y, double rad, double height, QPen pen)
{
    scene->addEllipse(x - rad / 2, height / 2 - y - rad / 2, rad, rad, pen);
}

void Model::setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene)
{
    QVector<double> column(solution.size());

    for (int i = 0; i < solution.size(); ++i)
        column[i] = solution[i].toDouble();

    double max = column[0], min = column[0];

    for (int i = 1; i < solution.size(); ++i)
    {
        if (max < column[i])
            max = column[i];
        if (column[i] < min)
            min = column[i];
    }

    QPainterPath *path = new QPainterPath;
    QPen red(Qt::red);

    double unitX = 0, unitY = 0, rad = height / 50.0;
    double sceneWidth = width - 2, sceneHeight = height - 2;

    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    unitY = (sceneHeight / 2 - rad / 2) / ABS(max);
    unitX = (sceneWidth - rad / 2) / solution.size();

    scene->addLine(0, height / 2, width, height / 2);
    scene->addLine(0, 0, 0, height);

    addPointAt(scene, unitX,  unitY * column[0], rad, sceneHeight, red);

    for (int i = 1; i < solution.size(); ++i)
    {
        addPointAt(scene, (i + 1) * unitX,  unitY * column[i], rad, sceneHeight, red);
        scene->addLine(i * unitX, sceneHeight / 2 - unitY * column[i - 1], (i + 1) * unitX, sceneHeight / 2 - unitY * column[i]);
    }

    scene->addPath(*path, red);
}

void Model::eraseSelectedTasks(QVector<int> numbersInHistory, int typeId)
{
    for (int i = 0; i < numbersInHistory.size(); ++i)
    {
        int taskId = 0;
        retrieveTaskFromHistory(taskId, typeId, numbersInHistory[i]);

        QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
        QSqlQuery query(db);

        query.prepare(DELETE_TASK);
        query.bindValue(":taskId", taskId);
        query.exec();
    }
    return;
}

double Model::bisection(double a, double b, double precision)
{
    double lb = -50,
            rb = 50;

    while ((a * lb - b) * (a * rb - b) > 0)
    {
        lb -= 10;
        rb += 10;
    }

    while (rb - lb > precision)
    {
        double m = (lb + rb) / 2;
        if ((a * lb - b) * (a * m - b) < 0)
            rb = m;
        else
            lb = m;
    }

    return (lb + rb) / 2;
}

InputCompleteness Model::createTask(QStringList lValues, QStringList rValues, int taskTypeId, int taskIdInDB)
{
    int dim = lValues.size();

    if (taskIdInDB)
        for (int k = 0; k < tasksInSession.size(); ++k)
        {
            if (tasksInSession[k].idInDB == taskIdInDB)
            {
                sessionIndexOfTaskInFocus = k;
                return INPUT_COMPLETE;
            }
        }

    if (!taskIsValid(lValues, rValues, taskTypeId))
    {
        return INPUT_INCOMPLETE_TASK;
    }

    Matrix matrix(dim);
    Vector column(dim);

    parseTask(lValues, rValues, matrix, column);

    Task task(taskIdInDB, taskTypeId);
    task.matrix = matrix;
    task.vector = column;

    sessionIndexOfTaskInFocus = tasksInSession.size();
    tasksInSession << task;

    return INPUT_COMPLETE;
}

bool Model::saveSelectedTasks(QVector<int> numbersInSession, bool all)
{
    bool savedAny = false;

    if (all)
    {
       numbersInSession.clear();
       for (int i = 0; i < tasksInSession.size(); ++i)
       {
           numbersInSession << i;
       }
    }
    foreach (int i, numbersInSession)
    {
        if (tasksInSession[i].idInDB == 0)
        {
            tasksInSession[i].idInDB = saveTaskFromSession(i);
            savedAny = true;
        }
    }

    return savedAny;
}
