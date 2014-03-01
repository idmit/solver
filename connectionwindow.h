#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QDialog>
#include <QHash>

#define CONNECTION_WINDOW_TITLE "Create a connection"

namespace Ui {
class ConnectionWindow;
}

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();

signals:
    /*
     * Sent when user specified all options for connection
     */
    void connectionOptionsSpecified();
    /*
     * Sent when quit button clicked
     */
    void quitButtonClicked();

public slots:
    /*
    (IN) drivers -- list of strings to be placed in driversComboBox
     */
    void refreshDriversCombo(QStringList const &drivers);
    /*
    (IN) options -- hash to be filled with named connection options:
    **** driver, dbname, username, password, hostname
     */
    void getConnectionOptions(QHash<QString, QString> &options) const;

private slots:
    void on_okButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::ConnectionWindow *ui;
};

#endif // CONNECTIONWINDOW_H
