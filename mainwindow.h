#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>

#define MAIN_WINDOW_TITLE "Solver"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void currentTaskTypeIndexChanged(int newIndex);
    void processTask(int taskNumberInHistory);

public slots:
    void refreshStatus(QString status, int timeout);
    void refreshTaskTypesCombo(QStringList &taskTypes);
    void refreshTaskHistoryList(QStringList &taskHistory);
    void currentTypeIndex(int &currentIndex);
    void selectedHistoryItemIndex(QVector<int> &selectedIndexes);

private slots:
    void on_taskTypesCombo_currentIndexChanged(int index);
    void on_newTaskButton_clicked();
    void on_taskHistoryList_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
