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
    QWidget(parent), mainWindow(0), connectionWindow(0), taskWindow(0), model(0)
{

}

void Controller::initialize(MainWindow *_mainWindow, ConnectionWindow *_connectionWindow, TaskWindow *_taskWindow, Model *_model)
{
    mainWindow = _mainWindow;
    connectionWindow = _connectionWindow;
    taskWindow = _taskWindow;
    model = _model;

    QObject::connect(connectionWindow, SIGNAL(connectionOptionsSpecified()), this, SLOT(processConnectionOptions()));
    QObject::connect(connectionWindow, SIGNAL(quitButtonClicked()), this, SLOT(quitApp()));
    QObject::connect(mainWindow, SIGNAL(quitButtonClicked()), this, SLOT(quitApp()));

    QObject::connect(this, SIGNAL(statusChanged(QString,int)), mainWindow, SLOT(refreshStatusBar(QString,int)));

    QObject::connect(mainWindow, SIGNAL(differentTaskTypeChosen(int)), this, SLOT(showTaskHistory(int)));

    QObject::connect(mainWindow, SIGNAL(showAllCheckBoxChanged(int)), this, SLOT(showTaskHistory(int)));

    QObject::connect(mainWindow, SIGNAL(processTask(int)), this, SLOT(showTaskWindow(int)));

    QObject::connect(taskWindow, SIGNAL(createButtonClicked(QStringList,QStringList)), this, SLOT(createTask(QStringList,QStringList)));
    QObject::connect(taskWindow, SIGNAL(solveButtonClicked(QStringList,QStringList)), this, SLOT(solveTask()));

    QObject::connect(model, SIGNAL(getMeta(QStringList,QHash<QString,QString>*)), this, SLOT(askMeta(QStringList,QHash<QString,QString>*)));
}

void Controller::alert(QString msg, QWidget *parent)
{
    QMessageBox msgBox(parent);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setText(msg);
    msgBox.exec();
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
    bool result = false;

    connectionWindow->getConnectionOptions(options);
    model->attemptToAddConnection(options, result);

    if (result)
    {
        statusChanged(CONNECTION_SUCC, STATUS_DURATION);
        showTaskTypes(result);
    }
    else
    {
        alert(CONNECTION_ERR, connectionWindow);
    }
    connectionWindow->setHidden(result);
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

    if (mainWindow->showAllCheckBoxChecked())
        model->retrieveTaskHistory(taskTypeId, taskHistory);
    else
        model->retrieveTaskSession(taskTypeId, taskHistory);

    mainWindow->refreshTaskHistoryList(taskHistory);
}

void Controller::showSolutionMethods(int taskTypeId)
{
    QStringList solutionMethods;

    model->retrieveSolutionMethods(taskTypeId, solutionMethods);
    taskWindow->refreshSolutionMethodsCombo(solutionMethods);
}

void Controller::showTaskWindow(int taskIndexInHistory)
{
    int taskTypeIndex = 0,
        taskTypeId    = 0,
        taskNumberInHistory = taskIndexInHistory + 1;
    bool taskIsNew = false;
    QStringList lValues, rValues;

    mainWindow->selectedTypesComboIndex(taskTypeIndex);
    taskTypeId = taskTypeIndex + 1;

    showSolutionMethods(taskTypeId);

    if (taskTypeId == LE_TYPE_ID)
        taskWindow->enableAddButtonAtFirstLine(false);
    else if (SLAE_TYPE_ID)
        taskWindow->enableAddButtonAtFirstLine(true);

    taskIsNew = taskNumberInHistory ? false : true;
    if (taskIsNew)
    {
        taskWindow->clearCoefficientsGroupBox();
        taskWindow->appendLine();
        taskWindow->enableCreationMode();
    }
    else
    {
        int taskIdInDB = 0;
        if (mainWindow->showAllCheckBoxChecked())
        {
            model->retrieveTaskFromHistory(taskIdInDB, taskTypeId, taskNumberInHistory, &lValues, &rValues);
            model->createTask(lValues, rValues, taskTypeId, taskIdInDB);
        }
        else
            model->retrieveTaskFromSession(taskTypeId, taskNumberInHistory, &lValues, &rValues);
        taskWindow->generateLines(lValues, rValues);
        taskWindow->enableSolutionMode();
    }

    taskWindow->show();
}

void Controller::showLastSolution()
{
    QStringList solution;
    model->retrieveSessionSolutionValues(solution);

    DialogWithGraphicsView dialog(taskWindow);
    QVBoxLayout *vert = new QVBoxLayout(&dialog), *solutionLayout = new QVBoxLayout(&dialog);
    QHBoxLayout *horz = new QHBoxLayout(&dialog);
    QGraphicsView *view = new QGraphicsView(&dialog);
    QLabel *result = new QLabel(solution.join(" "), &dialog);
    QPushButton *okButton = new QPushButton(OK_TEXT, &dialog), *saveButton = new QPushButton(IMAGE_BUTTON_TEXT, &dialog);
    QGroupBox *outputData = new QGroupBox(SOLUTION_GROUP_TEXT, &dialog);

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

    QObject::connect(&dialog, SIGNAL(setUpScene(int,int,QStringList,QGraphicsScene*)), this, SLOT(setUpScene(int,int,QStringList,QGraphicsScene*)));
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

void Controller::solveTask()
{
    int solutionMethodNumberInList = 0,
            solutionMethodIndex = 0,
            solutionMethodId = 0;

    taskWindow->selectedSolutionMethodsComboIndex(solutionMethodIndex);
    solutionMethodNumberInList = solutionMethodIndex + 1;
    model->retrieveSolutionMethodFromList(solutionMethodId, solutionMethodNumberInList);

    InputCompleteness inputCompleteness = model->solveTask(solutionMethodId);

    switch (inputCompleteness)
    {
    case INPUT_COMPLETE:
        showLastSolution();
        int index;
        mainWindow->selectedTypesComboIndex(index);
        showTaskHistory(index);
        taskWindow->hide();
        break;
    case INPUT_INVALID_META:
        alert(INVALID_META_MSG, taskWindow);
        break;
    case INPUT_INCOMPLETE_TASK:
        alert(INCOMPLETE_TASK_MSG, taskWindow);
        break;
    }
}

void Controller::askMeta(QStringList keys, QHash<QString, QString> *textMeta)
{
    QDialog dialog(taskWindow);
    QFormLayout form(&dialog);

    textMeta->clear();
    dialog.setWindowModality(Qt::WindowModal);

    form.addRow(new QLabel(ASK_META_MSG));

    QList<QLineEdit *> fields;
    for(int i = 0; i < keys.size(); ++i)
    {
        QLineEdit *lineEdit = new QLineEdit(&dialog);
        QString label = keys[i];
        form.addRow(label, lineEdit);

        fields << lineEdit;
    }

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
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

void Controller::setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene)
{
    model->setUpScene(width, height, solution, scene);
}

QDialog *Controller::createDialog(QString msg, QWidget *parent)
{
    QDialog *dialog = new QDialog(parent);
    QVBoxLayout *vert = new QVBoxLayout(dialog);
    QHBoxLayout *horz = new QHBoxLayout(dialog);
    QLabel *question = new QLabel(msg, dialog);
    QPushButton *yesButton = new QPushButton("Yes", dialog), *cancelButton = new QPushButton(CANCEL_TEXT, dialog);


    dialog->setWindowModality(Qt::WindowModal);
    dialog->setLayout(vert);

    vert->addWidget(question);

    horz->addWidget(yesButton);
    horz->addWidget(cancelButton);

    vert->addLayout(horz);

    QObject::connect(yesButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));

    return dialog;
}

void Controller::deleteHistoryItems()
{
    QVector<int> selectedIndexes(0), selectedNumbers(0);
    mainWindow->selectedHistoryListIndexes(selectedIndexes);
    for (int i = 0; i < selectedIndexes.size(); ++i)
    {
        selectedNumbers << selectedIndexes[i] + 1;
    }

    if (!selectedIndexes.isEmpty())
    {
        QDialog *dialog = createDialog(DELETE_WARNING, mainWindow);

        if (dialog->exec() == QDialog::Accepted)
        {
            int currentTypeIndex = 0, currentTypeId = 0;
            mainWindow->selectedTypesComboIndex(currentTypeIndex);
            currentTypeId = currentTypeIndex + 1;
            model->eraseSelectedTasks(selectedNumbers, currentTypeId);
            showTaskHistory(currentTypeIndex);
        }
    }
}

void Controller::saveSessionItems()
{
    QVector<int> selectedIndexes(0);

    if (mainWindow->showAllCheckBoxChecked())
        return;

    mainWindow->selectedHistoryListIndexes(selectedIndexes);

    if (!selectedIndexes.isEmpty())
    {
        int currentTypeIndex = 0;
        mainWindow->selectedTypesComboIndex(currentTypeIndex);
        bool savedAny = model->saveSelectedTasks(selectedIndexes);
        showTaskHistory(currentTypeIndex);
        if (savedAny) statusChanged(SAVE_SUCCESS_MSG, STATUS_DURATION);
    }
}


void Controller::createTask(QStringList lValues, QStringList rValues)
{
    removeRedundantData(lValues, rValues);
    if (lValues.size() == 0)
    {
        alert(EMPTY_TASK_MSG, taskWindow);
        return;
    }

    int index = 0;
    mainWindow->selectedTypesComboIndex(index);
    int typeId = index + 1;

    InputCompleteness inputCompleteness = model->createTask(lValues, rValues, typeId);

    switch (inputCompleteness)
    {
    case INPUT_COMPLETE:
    {
        int index = 0;
        mainWindow->selectedTypesComboIndex(index);
        showTaskHistory(index);
        taskWindow->hide();
    }
        break;
    case INPUT_INVALID_META:
        alert(INVALID_META_MSG, taskWindow);
        break;
    case INPUT_INCOMPLETE_TASK:
        alert(INCOMPLETE_TASK_MSG, taskWindow);
        break;
    }
}

void Controller::proposeToSaveSession()
{
    QDialog *dialog = createDialog("Do you want to save all tasks from this session?", mainWindow);

    QVector<int> selectedIndexes(mainWindow->count());
    if (selectedIndexes.size() == 0) return;
    for (int i = 0; i < selectedIndexes.size(); ++i)
        selectedIndexes[i] = i;

    if (dialog->exec() == QDialog::Accepted)
    {
        int currentTypeIndex = 0, currentTypeId = 0;
        mainWindow->selectedTypesComboIndex(currentTypeIndex);
        currentTypeId = currentTypeIndex + 1;
        model->saveSelectedTasks(selectedIndexes, true);
        showTaskHistory(currentTypeIndex);
    }
}

void Controller::quitApp()
{

    proposeToSaveSession();
    exit(0);
}
