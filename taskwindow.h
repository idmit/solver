#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>

#define TASK_WINDOW_TITLE "Edit your task"

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = 0);
    ~TaskWindow();

signals:
    void editButtonClicked();
    void solveButtonClicked(QStringList lValues, QStringList rValues);

public slots:
    void refreshSolutionMethods(QStringList &solutionMethods);
    void addLineAtIndex(int index, QString lValue = "", QString rValue = "");
    void removeLineAtIndex(int index);
    void appendLine();
    void addLine();
    void removeLine();
    void clear();

    void enableFirstAddButton(bool en);
    void refreshLines(QStringList lValues, QStringList rValues);

    void allowEdit();
    void forbidEdit();
    void showEditButton(bool en);
    void hideEditButton();

private slots:
    void on_solveButton_clicked();

    void on_editButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::TaskWindow *ui;
    int nextEmptyRow;
    bool firstAddButtonDisabled;
};

#endif // TASKWINDOW_H
