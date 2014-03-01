#include "dialogwithgraphicsview.h"
#include "ui_dialogwithgraphicsview.h"

#include <QLabel>

DialogWithGraphicsView::DialogWithGraphicsView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWithGraphicsView)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    resize(800, 400);
    on_backButton_clicked();
    QObject::connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveSceneAsImage()));
}

DialogWithGraphicsView::~DialogWithGraphicsView()
{
    delete ui;
}

void DialogWithGraphicsView::on_backButton_clicked()
{
    ui->backButton->hide();
    ui->graphicsView->hide();
    ui->visualizeButton->show();
    ui->scrollArea->show();
}

void DialogWithGraphicsView::on_visualizeButton_clicked()
{
    ui->backButton->show();
    ui->graphicsView->show();
    ui->visualizeButton->hide();
    ui->scrollArea->hide();
    resizeEvent(0);
}

void DialogWithGraphicsView::showEvent(QShowEvent *event)
{
    QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
    for (int k = 0; k < solutionsNumber; ++k)
    {
        emit needScene(ui->graphicsView->width(), ui->graphicsView->height(), scene);
    }
    if (event) event->accept();

}
void DialogWithGraphicsView::resizeEvent(QResizeEvent * event)
{
    showEvent(0);
    if (event) event->accept();
}

void DialogWithGraphicsView::saveSceneAsImage()
{
    QPixmap pixMap =  QPixmap::grabWidget(ui->graphicsView);
    /*
     * QPixmap::grabWidget is deprecated, use QWidget::grab() instead.
     * It is chosen to support older versions of Qt.
    QPixmap pixMap = graphicsView->grab();
    */
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
        pixMap.save(parsed.join("."));
        accept();
    }
}

void DialogWithGraphicsView::refreshSolution(QString solution)
{
    ui->solutionLayout->addWidget(new QLabel(solution, ui->scrollArea));
}

void DialogWithGraphicsView::refreshSolution(QStringList solution)
{
    foreach (QString val, solution)
    {
        ui->solutionLayout->addWidget(new QLabel(val, ui->scrollArea));
    }
}
