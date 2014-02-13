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

public slots:
    void refreshSolutionMethods(QStringList &solutionMethods);
    void appendLine();

private slots:
    void on_solveButton_clicked();

private:
    Ui::TaskWindow *ui;
    int nextEmptyRow;
};

#endif // TASKWINDOW_H
