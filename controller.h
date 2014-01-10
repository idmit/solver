#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>

class Controller : public QWidget
{
    Q_OBJECT
public:
    explicit Controller(QWidget *parent = 0);

signals:

public slots:
    void quitApp();
    void showConnectionWindow();
};

#endif // CONTROLLER_H
