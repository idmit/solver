#include "connectionwindow.h"
#include "ui_connectionwindow.h"

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    setWindowTitle(CONNECTION_WINDOW_TITLE);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::refreshDrivers(QStringList &drivers)
{
    ui->availableDriversCombo->clear();
    ui->availableDriversCombo->addItems(drivers);
}

void ConnectionWindow::getOptions(QHash<QString, QString> &options)
{
    options["driver"] = ui->availableDriversCombo->currentText();
    options["dbname"] = ui->dbnameLine->text();
    options["username"] = ui->usernameLine->text();
    options["password"] = ui->passwordLine->text();
    options["hostname"] = ui->hostnameLine->text();
}

void ConnectionWindow::on_okButton_clicked()
{
    emit optionsSpecified();
}


void ConnectionWindow::on_cancelButton_clicked()
{
    this->hide();
}

void ConnectionWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}
