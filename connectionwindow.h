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
    void optionsSpecified();

public slots:
    /* Refresh db drivers list */
    void refreshDrivers(QStringList &drivers);
    /* Export specified connection's options */
    void getOptions(QHash<QString, QString> &options);

private slots:
    void on_okButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::ConnectionWindow *ui;
};

#endif // CONNECTIONWINDOW_H
