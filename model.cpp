#include "model.h"
#include <QtSql>
#include <QGraphicsSimpleTextItem>

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
    case ODE_TYPE_ID:
    {
        QStringList row = lValues[0].split(" ", QString::SkipEmptyParts);
        QStringList rightSide = rValues[0].split(" ", QString::SkipEmptyParts);
        bool converts = true;

        if (row.size() != ODE_PARAMS_NUM || rightSide.size() != 1)
        {
            return false;
        }

        for (int k = 0; k < row.size(); ++k)
        {
            row[k].toDouble(&converts);
            if (!converts) return false;
        }
    }
    default:
        break;
    }

    return true;
}

void Model::parseTask(QStringList lValues, QStringList rValues, Matrix &matrix, Vector &column, int taskTypeId)
{
    if (taskTypeId == ODE_TYPE_ID)
    {
        QVector<double> row(0);

        row.clear();
        foreach (const QString &number, lValues[0].split(" ", QString::SkipEmptyParts))
        {
            row << number.toDouble();
        }
        matrix[0] = row;

        column[0] = rValues[0].split(" ", QString::SkipEmptyParts)[0].toDouble();
        return;
    }

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

    for (int i = 0; i < tasksInSession[k].solutions.size(); ++i)
    {
        saveSolution(tasksInSession[k].solutions[i], newTaskId);
    }

    return newTaskId;
}

int Model::retrieveSolutionFromSession(Task task, int solutionMethodId, QHash<QString, double> meta, QVector<double> *values)
{
    QVector<Solution> solutions;
    QVector<int> indexes;

    for (int i = 0; i < task.solutions.size(); ++i)
    {
        if (task.solutions[i].methodId == solutionMethodId)
        {
            solutions << task.solutions[i];
            indexes << i;
        }
    }

    for (int i = 0; i < solutions.size(); ++i)
    {
        if (solutions[i].meta.values() == meta.values())
        {
            if (values)
                *values = solutions[i].values;
            return indexes[i];
        }
    }

    return -1;
}

bool Model::retrieveSolutionFromDB(int taskIdInDB, int solutionMethodId, QHash<QString, double> meta,  QVector<double> *solution)
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
        QStringList sortedKeys = sort(meta.keys());
        while (q.next())
        {
            if (meta[sortedKeys[i]] != q.value(1).toDouble())
            {
                found = false;
            }
            i++;
        }

        if (found)
        {
            solutionExists = true;
            if (!solution) return solutionExists;
            q.prepare(SELECT_VALUES);
            q.bindValue(":solutionId", solutionId);
            q.exec();
            while (q.next())
            {
                solution->append(q.value(0).toDouble());
            }
        }
    }
    return solutionExists;
}

void Model::retrieveTaskSolutionsBySessionIndex(int index, QStringList &solutionValues)
{
    if (index < 0)
        index = sessionIndexOfTaskInFocus;

    foreach (Solution solution, tasksInSession[index].solutions)
    {
        QString values = "";
        foreach (double val, solution.values)
        {
            values += QString::number(val) + " ";
        }
        solutionValues << values;
    }
}

void Model::retrieveLastSolution(QString &values)
{
    values = "";
    foreach (double val, tasksInSession[sessionIndexOfTaskInFocus].solutions[lastSolutionIndex].values)
    {
        values += QString::number(val) + " ";
    }
}

void Model::saveSolution(Solution &solution, int taskIdInDB)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QSqlQuery query(db);

    query.prepare(INSERT_SOLUTION);
    query.bindValue(":taskId", taskIdInDB);
    query.bindValue(":methodId", solution.methodId);
    query.exec();

    int solutionId = query.lastInsertId().toInt();

    for (int i = 0; i < solution.values.size(); ++i)
    {
        query.prepare(INSERT_VALUE);
        query.bindValue(":solutionId", solutionId);
        query.bindValue(":value", solution.values[i]);
        query.exec();
    }

    qDebug() << query.lastError();

    QStringList sortedKeys = sort(solution.meta.keys());

    for (int i = 0; i < sortedKeys.size(); ++i)
    {
        query.prepare(INSERT_META);
        query.bindValue(":solutionId", solutionId);
        query.bindValue(":name", sortedKeys[i]);
        query.bindValue(":value", solution.meta[sortedKeys[i]]);
        query.exec();
    }

    solution.id = solutionId;
    solution.isSaved = true;
}

bool Model::metaIsValid(QHash<QString, QString> textMeta, QHash<QString, double> &meta)
{
    if (textMeta.size() == 0) return false;

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
    QStringList valuesAsList;
    QVector<double> values;

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
    case EULER_METHOD_ID:
    {
        QStringList keys;
        keys << "From" << "To" << "Step" << "Initial value";
        emit getMeta(keys, &textMeta);
        if (!metaIsValid(textMeta, meta))
            return INPUT_INVALID_META;
        if (meta["To"] - meta["From"] <= 0 || meta["Step"] <= 0 || meta["Step"] > meta["To"] - meta["From"])
            return INPUT_INVALID_META;
        solution.meta = meta;
        QVector<double> dummy(ceil((meta["To"] - meta["From"]) / meta["Step"]));
        result = dummy;
    }
        break;
    default:
        break;
    }


    int index = retrieveSolutionFromSession(tasksInSession[sessionIndexOfTaskInFocus], solutionMethodId, meta, &values);

    if (index != -1)
    {
        lastSolutionIndex = index;
        return INPUT_COMPLETE;
    }
    else
    {
        if (tasksInSession[sessionIndexOfTaskInFocus].idInDB != 0)
            if (retrieveSolutionFromDB(tasksInSession[sessionIndexOfTaskInFocus].idInDB, solutionMethodId, meta, &values))
            {
                solution.values = values;
                solution.isSaved = true;
                lastSolutionIndex = tasksInSession[sessionIndexOfTaskInFocus].solutions.size();
                tasksInSession[sessionIndexOfTaskInFocus].solutions << solution;
                return INPUT_COMPLETE;
            }
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
    case EULER_METHOD_ID:
        result = euler(meta["From"], meta["To"], meta["Step"], meta["Initial value"], matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
    default:
        break;
    }

    solution.values = result;
    lastSolutionIndex = tasksInSession[sessionIndexOfTaskInFocus].solutions.size();
    if (tasksInSession[sessionIndexOfTaskInFocus].idInDB != 0)
        saveSolution(solution, tasksInSession[sessionIndexOfTaskInFocus].idInDB);
    tasksInSession[sessionIndexOfTaskInFocus].solutions << solution;
    return INPUT_COMPLETE;
}

static void addPointAt(QGraphicsScene *scene, double x, double y, double rad, double height, QPen pen)
{
    scene->addEllipse(x - rad / 2, height / 2 - y - rad / 2, rad, rad, pen);
}

void Model::setUpScene(int width, int height, QGraphicsScene *scene)
{
    QVector<QPen> pens; pens << QPen(Qt::red) << QPen(Qt::green) << QPen(Qt::blue);
    int p = -1;

    double max = solutionsToShow[0].values[0], maxW = solutionsToShow[0].meta["To"] - solutionsToShow[0].meta["From"];
    foreach (Solution solution, solutionsToShow)
    {
        for (int k = 0; k < solution.values.size(); ++k)
        {
            if (ABS(solution.values[k]) > max)
                max = ABS(solution.values[k]);
        }
        if (solution.meta["To"] - solution.meta["From"] > maxW)
            maxW = solution.meta["To"] - solution.meta["From"];
    }

    double unitX = 0, unitY = 0, rad = height / 50.0, halfX, start, step;
    double sceneWidth = width - 2, sceneHeight = height - 2;

    if (solutionsToShow[0].methodId == EULER_METHOD_ID)
    {
        halfX = sceneWidth / 2;
        unitX = sceneWidth / maxW;
    }
    else
    {
        step = 1;
        halfX = 0;
        start = 0;
        if (solutionsToShow[0].values.size() == 1)
            unitX = (sceneWidth - rad / 2);
        else
            unitX = (sceneWidth - rad / 2) / (solutionsToShow[0].values.size() - 1);
    }
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    unitY = (sceneHeight / 2 - rad / 2) / max;

    foreach (Solution solution, solutionsToShow)
    {
        p++;
        QVector<double> column = solution.values;

        QPainterPath *path = new QPainterPath;

        if (solutionsToShow[0].methodId == EULER_METHOD_ID)
        {
            start = solution.meta["From"];
            step = solution.meta["Step"];
        }

        scene->addLine(0, height / 2, width, height / 2);
        scene->addLine(halfX, 0, halfX, height);
        QGraphicsSimpleTextItem *x = scene->addSimpleText("x");
        x->setPos(sceneWidth - x->boundingRect().width(), sceneHeight / 2);
        QGraphicsSimpleTextItem *y = scene->addSimpleText("y");
        y->setPos(halfX + 2, 0);

        if (column.size() == 1) addPointAt(scene, unitX,  unitY * column[0], rad, sceneHeight, pens[p]);

        for (int i = 0; i < column.size() - 1; ++i)
        {
            scene->addLine(halfX + (start + i * step) * unitX, sceneHeight / 2 - unitY * column[i], halfX + (start + (i + 1) * step) * unitX, sceneHeight / 2 - unitY * column[i + 1], pens[p]);
            if (step == 1)
            {
                QGraphicsSimpleTextItem *x = scene->addSimpleText(QString("%1").arg(i));
                x->setPos(i * unitX, sceneHeight / 2);
            }
        }

        scene->addPath(*path, pens[p]);
    }
}

void Model::eraseSelectedTasks(QVector<int> numbersInHistory, int typeId)
{
    for (int i = 0; i < numbersInHistory.size(); ++i)
    {
        int taskId = 0;
        retrieveTaskFromHistory(taskId, typeId, numbersInHistory[i]);

        for (int k = 0; k < tasksInSession.size(); ++k)
        {
              if (tasksInSession[k].idInDB == taskId)
              {
                  tasksInSession[k].idInDB = 0;
                  for (int j = 0; j < tasksInSession[k].solutions.size(); ++j)
                  {
                      tasksInSession[k].solutions[j].id = 0;
                      tasksInSession[k].solutions[j].isSaved = false;
                  }
              }
        }

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
    int expectedDim = taskTypeId == ODE_TYPE_ID ? ODE_PARAMS_NUM : lValues.size();
    int dim = expectedDim;

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

    parseTask(lValues, rValues, matrix, column, taskTypeId);

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

QVector<int> Model::indexesOfUnsavedSessionTasks()
{
    QVector<int> unsavedIndexes;

    for (int i = 0; i < tasksInSession.size(); ++i)
    {
        if (tasksInSession[i].idInDB == 0)
            unsavedIndexes << i;
    }

    return unsavedIndexes;
}

Vector Model::euler(double start, double end, double step, double initval, double expval, double sinval, double cosval, double constval)
{
    int pointNumber = ceil((end - start) / step) + 1;
    Vector y(pointNumber);

    y[0] = initval;

    for (int i = 1; i < pointNumber; ++i)
    {
        y[i] = y[i - 1] + step * (exp(expval * (start + (i - 1) * step))*(sinval * sin((start + (i - 1) * step)) + constval + cosval * cos((start + (i - 1) * step))));
    }

    return y;
}
