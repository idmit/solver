#ifndef DIALOGWITHGRAPHICSVIEW_H
#define DIALOGWITHGRAPHICSVIEW_H

#include <QDialog>
#include <QShowEvent>
#include <QResizeEvent>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QCloseEvent>

namespace Ui {
class DialogWithGraphicsView;
}

class DialogWithGraphicsView : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWithGraphicsView(QWidget *parent = 0);
    ~DialogWithGraphicsView();

    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent * event);
    void closeEvent(QCloseEvent * event) { event->ignore(); }
    int solutionsNumber;

public slots:
    void saveSceneAsImage();
    /*
    (IN) solution -- string to be set as label
     */
    void refreshSolution(QString solution);
    /*
    (IN) solution -- list of strings, each to be set as label
     */
    void refreshSolution(QStringList solution);

signals:
    /*
     * Sent when window is about to show scene, passing width and height of view and pointer to write by
     */
    void needScene(int width, int height, QGraphicsScene *scene);

private slots:
    void on_backButton_clicked();
    void on_visualizeButton_clicked();

private:
    Ui::DialogWithGraphicsView *ui;
};

#endif // DIALOGWITHGRAPHICSVIEW_H
