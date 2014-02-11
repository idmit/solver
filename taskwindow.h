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

public slots:
    void refreshSolutionMethods(QStringList &solutionMethods);

private slots:
    void on_pushButton_clicked();

private:
    Ui::TaskWindow *ui;
};

#endif // TASKWINDOW_H
