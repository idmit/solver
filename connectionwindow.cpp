#include "connectionwindow.h"
#include "ui_connectionwindow.h"

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);

    setWindowTitle(CONNECTION_WINDOW_TITLE);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::refreshDriversCombo(QStringList const &drivers)
{
    ui->availableDriversCombo->clear();
    ui->availableDriversCombo->addItems(drivers);
}

void ConnectionWindow::getConnectionOptions(QHash<QString, QString> &options) const
{
    options["driver"] = ui->availableDriversCombo->currentText();
    options["dbname"] = ui->dbnameLine->text();
    options["username"] = ui->usernameLine->text();
    options["password"] = ui->passwordLine->text();
    options["hostname"] = ui->hostnameLine->text();
}

void ConnectionWindow::on_okButton_clicked()
{
    emit connectionOptionsSpecified();
}

void ConnectionWindow::on_quitButton_clicked()
{
    exit(0);
}
