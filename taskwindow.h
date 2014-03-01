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
    /*
     Sent when edit button clicked
     */
    void editButtonClicked();
    /*
     Sent when solve button clicked, passing filled data
     */
    void solveButtonClicked(QStringList lValues, QStringList rValues);
    /*
     Sent when create button clicked, passing filled data
     */
    void createButtonClicked(QStringList lValues, QStringList rValues);

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

    /*
    (OUT) -- index of currently selected solution method in solutionMethodsComboBox
     */
    void selectedSolutionMethodsComboIndex(int &solutionMethodIndex) const;

    void enableSolutionMode();
    void enableCreationMode();
    void setVisibleHeader(bool vis);
private slots:
    void on_solveButton_clicked();
    void on_editButton_clicked();
    void on_closeButton_clicked();

    void on_createButton_clicked();

private:
    Ui::TaskWindow *ui;
    /*
    Index of next empty row in coefficientsGroupBox
     */
    int nextEmptyRow;
    bool firstAddButtonDisabled;
};

#endif // TASKWINDOW_H
