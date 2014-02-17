#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QShowEvent>
#include <QResizeEvent>
#include <QFileDialog>

#define TASK_WINDOW_TITLE "Edit your task"

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = 0);
    ~TaskWindow();

signals:
    void editButtonClicked();
    void solveButtonClicked(QStringList lValues, QStringList rValues);

public slots:
    /*
    (IN) solutionMethods -- list of strings to be placed in solutionMethodsComboBox
     */
    void refreshSolutionMethodsCombo(QStringList const &solutionMethods);

    /* Line is a row of five widgets: two text fields, equal sign and plus/minus buttons */

    /*
    (IN) index -- index of row of coefficientsGroupBox to be filled with new line
    (IN) lValue -- value of first text field
    (IN) rValue -- value if second text field
     */
    void addLineAtIndex(int index, QString const &lValue = "", QString const &rValue = "");
    void addLineBelowClicked();

    /*
    (IN) index -- index of row of coefficientsGroupBox to be removed
     */
    void removeLineAtIndex(int index);
    void removeClickedLine();
    
    void appendLine();
    /*
    (IN) lValues -- list of strings to be placed in left fields of generated set of lines
    (IN) lValues -- list of strings to be placed in right fields of generated set of lines
     */
    void generateLines(QStringList const &lValues, QStringList const &rValues);
    void clearCoefficientsGroupBox();

    /*
    (IN) enabled -- sets plus button of the line indexed zero enabled (true) or disabled (false)
     */
    void enableAddButtonAtFirstLine(bool enabled);
    
    void makeCoefficientsGroupBoxEditable();
    void forbidEditOfCoefficientsGroupBox();

    /*
    (IN) visible -- sets editButton visible (true) or hidden (false)
     */
    void showEditButton(bool visible);
    void hideEditButton();

    /*
    (OUT) -- index of currently selected solution method in solutionMethodsComboBox
     */
    void selectedSolutionMethodsComboIndex(int &solutionMethodIndex) const;

private slots:
    void on_solveButton_clicked();
    void on_editButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::TaskWindow *ui;
    /*
    Index of next empty row in coefficientsGroupBox
     */
    int nextEmptyRow;
    bool firstAddButtonDisabled;
};

class DialogWithGraphicsView : public QDialog
{
    Q_OBJECT
public:
    DialogWithGraphicsView(QWidget *parent) : QDialog(parent) {}
    virtual ~DialogWithGraphicsView() {}

    void showEvent(QShowEvent *event)
    {
        QGraphicsScene *scene = new QGraphicsScene(graphicsView);
        graphicsView->setScene(scene);
        emit setUpScene(graphicsView->width(), graphicsView->height(), solution, scene);
        if (event) event->accept();
    }
    void resizeEvent(QResizeEvent * event)
    {
        showEvent(0);
        event->accept();
    }

    QGraphicsView *graphicsView;
    /*
    List of strings to be displayed one by one as a result. It is processed to get an image
     */
    QStringList solution;

public slots:
    void saveSceneAsImage()
    {
        QPixmap pixMap = graphicsView->grab();
        QString filename = QFileDialog::getSaveFileName(this);
        if (!filename.isEmpty())
        {
            while(filename.endsWith('.'))
                filename.chop(1);
            QStringList parsed = filename.split(".");
            if (parsed.size() == 1 || parsed[parsed.size() - 1] != "png")
            {
                parsed << "png";
            }
            pixMap.save(parsed.join('.'));
            accept();
        }
    }

signals:
    void setUpScene(int width, int height, QStringList solution, QGraphicsScene *scene);
};

#endif // TASKWINDOW_H
