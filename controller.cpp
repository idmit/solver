#include "controller.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QGroupBox>

Controller::Controller(QWidget *parent) :
    QWidget(parent)
{

}

void Controller::initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, TaskWindow *_taskWindow, Model *_model)
{
    mainWindow = _mainWindow;
    connectionWindow = _connectionWindow;
    taskWindow = _taskWindow;
    model = _model;

    QObject::connect(connectionWindow, SIGNAL(connectionOptionsSpecified()), this, SLOT(processConnectionOptions()));

    QObject::connect(model, SIGNAL(statusChanged(QString,int)), this, SIGNAL(statusChanged(QString,int)));
    QObject::connect(this, SIGNAL(statusChanged(QString,int)), mainWindow, SLOT(refreshStatus(QString,int)));

    QObject::connect(mainWindow, SIGNAL(currentTaskTypeIndexChanged(int)), this, SLOT(showTaskHistory(int)));

    QObject::connect(mainWindow, SIGNAL(processTask(int)), this, SLOT(showTaskWindow(int)));

    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), taskWindow, SLOT(allowEdit()));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), taskWindow, SLOT(hideEditButton()));
    QObject::connect(taskWindow, SIGNAL(editButtonClicked()), model, SLOT(makeTaskNew()));

    QObject::connect(taskWindow, SIGNAL(solveButtonClicked(QStringList,QStringList)), this, SLOT(processTask(QStringList,QStringList)));

    QObject::connect(model, SIGNAL(askMeta(QStringList,QHash<QString,QString>*)), this, SLOT(askMeta(QStringList,QHash<QString,QString>*)));
    QObject::connect(model, SIGNAL(alert(QString,int)), this, SLOT(alert(QString,int)));
}

/* CONNECTION CREATION BEGIN */

void Controller::showConnectionWindow()
{
    QStringList drivers;

    model->retrieveDrivers(drivers);
    connectionWindow->refreshDriversCombo(drivers);

    connectionWindow->show();
    connectionWindow->setFocus();
}

void Controller::processConnectionOptions()
{
    QHash<QString, QString> options;
    QMessageBox msgBox(connectionWindow);
    bool result = false;

    connectionWindow->getConnectionOptions(options);
    model->attemptToAddConnection(options, result);

    if (result)
    {
        statusChanged(CONNECTION_SUCC, 2000);
    }
    else
    {
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setText(CONNECTION_ERR);
        msgBox.exec();
    }
    connectionWindow->setHidden(result);
    showTaskTypes(result);
}

/* CONNECTION CREATION END */

void Controller::showTaskTypes(bool connectionExists)
{
    QStringList taskTypes;

    if (!connectionExists)
        return;

    model->retrieveTaskTypes(taskTypes);
    mainWindow->refreshTaskTypesCombo(taskTypes);
}

void Controller::showTaskHistory(int taskTypeIndex)
{
    QStringList taskHistory;
    int taskTypeId = taskTypeIndex + 1;

    model->retrieveTaskHistory(taskTypeId, taskHistory);
    mainWindow->refreshTaskHistoryList(taskHistory);
}

void Controller::showSolutionMethods(int taskTypeId)
{
    QStringList solutionMethods;

    model->retrieveSolutionMethods(taskTypeId, solutionMethods);
    taskWindow->refreshSolutionMethods(solutionMethods);
}

void Controller::showTaskWindow(int taskIndexInHistory)
{
    int taskTypeIndex = 0,
        taskTypeId    = 0,
        taskNumberInHistory = taskIndexInHistory + 1;
    bool taskIsNew = false;
    QStringList lValues, rValues;

    mainWindow->currentTypeIndex(taskTypeIndex);
    taskTypeId = taskTypeIndex + 1;

    showSolutionMethods(taskTypeId);

    if (taskTypeId == LE_TYPE_ID)
        taskWindow->enableFirstAddButton(false);
    else if (SLAE_TYPE_ID)
        taskWindow->enableFirstAddButton(true);

    taskIsNew = taskNumberInHistory ? false : true;
    if (taskIsNew)
    {
        taskWindow->clear();
        taskWindow->appendLine();
        model->regTask(0, taskTypeId, true);
    }
    else
    {
        int taskId = 0;
        model->retrieveTaskFromHistory(taskId, taskTypeId, taskNumberInHistory, lValues, rValues);
        taskWindow->refreshLines(lValues, rValues);
        model->regTask(taskId, taskTypeId, false);
    }

    taskWindow->showEditButton(!taskIsNew);
    if (!taskIsNew)
        taskWindow->forbidEdit();
    else
        taskWindow->allowEdit();

    taskWindow->show();
}

void Controller::showSolution(int solutionMethodId)
{
    QStringList solution;
    model->retrieveSolutionForProcessedTask(solutionMethodId, &solution);

    DialogWithGraphicsView dialog(taskWindow);
    QVBoxLayout *vert = new QVBoxLayout(&dialog), *solutionLayout = new QVBoxLayout(&dialog);
    QHBoxLayout *horz = new QHBoxLayout(&dialog);
    QGraphicsView *view = new QGraphicsView(&dialog);
    QLabel *result = new QLabel(solution.join(' '), &dialog);
    QPushButton *okButton = new QPushButton("OK", &dialog), *saveButton = new QPushButton("Save as Image", &dialog);
    QGroupBox *outputData = new QGroupBox("Solution", &dialog);

    dialog.graphicsView = view;
    dialog.solution = solution;

    dialog.setWindowModality(Qt::WindowModal);
    dialog.resize(800, 400);
    dialog.setLayout(vert);

    solutionLayout->addWidget(view);
    solutionLayout->addWidget(result);
    outputData->setLayout(solutionLayout);

    vert->addWidget(outputData);

    horz->addWidget(okButton);
    horz->addWidget(saveButton);

    vert->addLayout(horz);

    QObject::connect(&dialog, SIGNAL(draw(int,int,QStringList,QGraphicsScene*)), this, SLOT(draw(int,int,QStringList,QGraphicsScene*)));
    QObject::connect(okButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(saveButton, SIGNAL(clicked()), &dialog, SLOT(saveSceneAsImage()));

    dialog.exec();
}

void Controller::removeRedundantData(QStringList &lValues, QStringList &rValues)
{
    for (int i = 0; i < lValues.size(); ++i)
    {
        while(lValues[i].endsWith(' '))
            lValues[i].chop(1);
        while(rValues[i].endsWith(' '))
            rValues[i].chop(1);
        if (lValues[i].isEmpty() || rValues[i].isEmpty())
        {
            lValues.removeAt(i);
            rValues.removeAt(i);
            i--;
        }
    }
}

void Controller::processTask(QStringList lValues, QStringList rValues)
{
    removeRedundantData(lValues, rValues);

    if (lValues.size() == 0)
    {
        alert("Your task is empty", 3);
        return;
    }

    int solutionMethodNumberInList = 0,
            solutionMethodIndex = 0,
            solutionMethodId = 0;

    taskWindow->currentSolutionMethodIndex(solutionMethodIndex);
    solutionMethodNumberInList = solutionMethodIndex + 1;
    model->retrieveSolutionMethodFromList(solutionMethodId, solutionMethodNumberInList);

    if (model->solveTask(lValues, rValues, solutionMethodId))
    {
        showSolution(solutionMethodId);

        showTaskHistory(model->processedTaskTypeId() - 1);
        taskWindow->hide();
    }
}

void Controller::askMeta(QStringList keys, QHash<QString, QString> *textMeta)
{
    QDialog dialog(taskWindow);
    QFormLayout form(&dialog);

    textMeta->clear();
    dialog.setWindowModality(Qt::WindowModal);

    form.addRow(new QLabel("Provide some additional information:"));

    QList<QLineEdit *> fields;
    for(int i = 0; i < keys.size(); ++i)
    {
        QLineEdit *lineEdit = new QLineEdit(&dialog);
        QString label = keys[i];
        form.addRow(label, lineEdit);

        fields << lineEdit;
    }

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {
        for(int i = 0; i < fields.size(); ++i)
        {
            textMeta->insert(keys[i], fields[i]->text());
        }
    }
}

void Controller::alert(QString msg, int id)
{
    QWidget *parent = 0;

    switch (id)
    {
    case 3:
        parent = taskWindow;
        break;
    case 0:
        parent = dynamic_cast<QWidget *>(QObject::sender());
    default:
        break;
    }

    QMessageBox msgBox(parent);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setText(msg);
    msgBox.exec();
}

void Controller::draw(int width, int height, QStringList solution, QGraphicsScene *scene)
{
    model->setUpScene(width, height, solution, scene);
}

void Controller::deleteHistoryItem()
{
    QVector<int> selectedIndexes(0), selectedNumbers(0);
    mainWindow->selectedHistoryItemIndex(selectedIndexes);
    for (int i = 0; i < selectedIndexes.size(); ++i)
    {
        selectedNumbers << selectedIndexes[i] + 1;
    }

    if (!selectedIndexes.isEmpty())
    {
        QDialog dialog(mainWindow);
        QVBoxLayout *vert = new QVBoxLayout(&dialog);
        QHBoxLayout *horz = new QHBoxLayout(&dialog);
        QLabel *question = new QLabel("You are going to permanently delete some tasks. Are you sure?", &dialog);
        QPushButton *yesButton = new QPushButton("Yes", &dialog), *cancelButton = new QPushButton("Cancel", &dialog);


        dialog.setWindowModality(Qt::WindowModal);
        dialog.setLayout(vert);

        vert->addWidget(question);

        horz->addWidget(yesButton);
        horz->addWidget(cancelButton);

        vert->addLayout(horz);

        QObject::connect(yesButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));

        if (dialog.exec() == QDialog::Accepted)
        {
            int currentTypeIndex = 0, currentTypeId = 0;
            mainWindow->currentTypeIndex(currentTypeIndex);
            currentTypeId = currentTypeIndex + 1;
            model->removeSelectedTasks(selectedNumbers, currentTypeId);
            showTaskHistory(currentTypeIndex);
        }
    }
}
