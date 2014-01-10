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

public slots:
    void refreshStatus(QString status, int timeout);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
