#include "taskwindow.h"
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);

    setWindowTitle(TASK_WINDOW_TITLE);
}

TaskWindow::~TaskWindow()
{
    delete ui;
}
