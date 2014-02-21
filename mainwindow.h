#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QListView>

#define MAIN_WINDOW_TITLE "Solver"
#define FIRST_MENU_ITEM "File"
#define CONNECT_MENU_ACTION "Connect to server"
#define SECOND_MENU_ITEM "History"
#define DELETE_MENU_ITEM "Delete selected history item"

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
    void differentTaskTypeChosen(int newIndex);
    void processTask(int taskNumberInHistory);

public slots:
    /*
    (IN) status -- string to be placed in status bar
    (IN) timeout -- duration of string displaying in status bar
     */
    void refreshStatusBar(QString const &status, int timeout);
    /*
    (IN) taskTypes -- list of strings to be placed in taskTypesComboBox
     */
    void refreshTaskTypesCombo(QStringList const &taskTypes);
    /*
    (IN) taskHistory -- list of strings to be placed in taskHistoryList
     */
    void refreshTaskHistoryList(QStringList const &taskHistory);

    /*
    (OUT) selectedIndex -- index of currently selected type in taskTypesComboBox
     */
    void selectedTypesComboIndex(int &selectedIndex) const;
    /*
    (OUT) selectedIndexes -- vector of indexes of currently selected items in taskHistoryList
     */
    void selectedHistoryListIndexes(QVector<int> &selectedIndexes) const;

private slots:
    void on_taskTypesCombo_currentIndexChanged(int index);
    void on_newTaskButton_clicked();
    void on_taskHistoryList_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
