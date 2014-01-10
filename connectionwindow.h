#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QDialog>
#include <QHash>
#include <QCloseEvent>

#define CONNECTION_WINDOW_TITLE "Create a connection"

namespace Ui {
class ConnectionWindow;
}

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    void closeEvent(QCloseEvent *event);
    ~ConnectionWindow();

signals:
    void optionsSpecified();

public slots:
    /* Refresh db drivers list */
    void refreshDrivers(QStringList &drivers);
    /* Export chosen connection's options */
    void getOptions(QHash<QString, QString> &options);

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::ConnectionWindow *ui;
};

#endif // CONNECTIONWINDOW_H
