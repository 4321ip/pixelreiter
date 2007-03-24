#include "MainWindow.h"
#include "ZoomWidget.h"
#include "ColorCount.h"
#include "SelWin.h"
#include "ColorCountModel.h"
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QSettings>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QComboBox>
#include <QLabel>
#include <QModelIndex>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QTableView>
#include <QTreeView>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
    setupUi(this);

    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(exportCurrentAct, SIGNAL(triggered()), this, SLOT(exportCurrent()));
    connect(importScreenshotAct, SIGNAL(triggered()), this, SLOT(importScreenshot()));
    connect(grabAct, SIGNAL(triggered(bool)), this, SLOT(toggleGrab(bool)));

    QAction* colorDockAction = colorsDock->toggleViewAction();
    colorDockAction->setText(tr("show &colors"));
    settingsMenu->addAction(colorDockAction);
    
     QActionGroup* gridGroup = new QActionGroup(this);
     gridGroup->addAction(actionGridNone);
     gridGroup->addAction(actionGridWhite);
     gridGroup->addAction(actionGridBlack);
     connect(gridGroup, SIGNAL(triggered(QAction*)), this, SLOT(toggleShowGrid(QAction*)));
     
     connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
     connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));

//      intervalBox = new QComboBox;
//      intervalBox->insertItem(0, "30ms", 30);
//      intervalBox->insertItem(1, "100ms", 100);
//      intervalBox->insertItem(2, "500ms", 500);
//      intervalBox->insertItem(3, "1s",   1000);
//      intervalBox->insertItem(4, "2s",   2000);
//      intervalBox->insertItem(5, "3s",   3000);
//      intervalBox->insertItem(6, "4s",   4000);
//      intervalBox->insertItem(7, "5s",   5000);
//      intervalBox->insertItem(8, "7s",   7000);
//      intervalBox->insertItem(9, "10s", 10000);
// 
//      fileToolBar->addSeparator();
//      fileToolBar->addWidget(new QLabel("Update:"));
//      fileToolBar->addWidget(intervalBox);
//      connect(intervalBox, SIGNAL(activated(int)), this, SLOT(intervalChanged(int)));

    selWin = new SelWin;

    zoomWidget = new ZoomWidget(selWin);
    setCentralWidget(zoomWidget);

    m_colorCountModel = new ColorCountModel;

    connect(zoomWidget, SIGNAL(colorsChanged(QList<ColorCount>)), m_colorCountModel, SLOT(setColors(QList<ColorCount>))); 
    connect(m_colorCountModel, SIGNAL(layoutChanged()), this, SLOT(colorsTableChanged()));

    colorsTable->setModel(m_colorCountModel);
    colorsTable->setIndentation(0);
    colorsTable->setAlternatingRowColors(true);
    colorsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(colorsTable->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(colorClicked(QModelIndex)));
    connect(zoomWidget, SIGNAL(markColorChanged( const QColor& )), this, SLOT(changeColor( const QColor& )));

    connect(copyColorButton, SIGNAL(clicked()), this, SLOT(copyColor()));
    
    readSettings();
    
    statusBar()->showMessage(tr("Ready"));
}
void MainWindow::toggleGrab(bool i) {
    if(i)
        zoomWidget->play();
    else
        zoomWidget->pause();
}

void MainWindow::toggleShowGrid(QAction* action) {
    if(action==actionGridNone) {
        zoomWidget->setGridColor(QColor());
    } else if(action==actionGridWhite) {
        zoomWidget->setGridColor(QColor(Qt::white));
    } else if(action==actionGridBlack) {
        zoomWidget->setGridColor(QColor(Qt::black));
    }
}

void MainWindow::zoomIn() {
    zoomWidget->setZoomFactor(zoomWidget->zoomFactor()+1);
    statusBar()->showMessage(tr("Zoom: %1x").arg(zoomWidget->zoomFactor()), 1000);
    QCoreApplication::processEvents();
    zoomWidget->grabPixmap();
    zoomWidget->update();
}
void MainWindow::zoomOut() {
    if(zoomWidget->zoomFactor()>1) {
        zoomWidget->setZoomFactor(zoomWidget->zoomFactor()-1);
    }
    statusBar()->showMessage(tr("Zoom: %1x").arg(zoomWidget->zoomFactor()), 1000);
    QCoreApplication::processEvents();
    zoomWidget->grabPixmap();
    zoomWidget->update();
}

void MainWindow::intervalChanged(int i) {
    zoomWidget->setUpdateInterval(intervalBox->itemData(i).toInt());
}

void MainWindow::colorClicked(const QModelIndex & index) {
    if(index.isValid()) 
        zoomWidget->setMarkColor(m_colorCountModel->color(index.row()));
}
void MainWindow::colorsTableChanged() {
     colorClicked(colorsTable->selectionModel()->currentIndex());
}
void MainWindow::changeColor( const QColor & color) {
     QModelIndex index = m_colorCountModel->index(color.rgb());
     colorsTable->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}


void MainWindow::copyColor() 
{
     QClipboard *clipboard = QApplication::clipboard();
     QColor c(m_colorCountModel->color(colorsTable->selectionModel()->currentIndex().row()));
     clipboard->setText(c.name());
}
void MainWindow::readSettings()
{
    QSettings settings("NikoSams", "PixelReiter");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    restoreState(settings.value("state").toByteArray());
    resize(size);
    move(pos);
    selWin->move(settings.value("selWinPos", QPoint(0, 0)).toPoint());

    zoomWidget->setZoomFactor(settings.value("zoomFactor", 10).toInt());

//     int i = intervalBox->findData(settings.value("updateInterval", 1000).toInt());
//     if(i!=-1) {
//         intervalBox->setCurrentIndex(i);
//         zoomWidget->setUpdateInterval(settings.value("updateInterval", 1000).toInt());
//     } 
    zoomWidget->setGridColor(settings.value("gridColor", QColor()).value<QColor>());
    if(zoomWidget->gridColor()==QColor(Qt::black)) {
        actionGridBlack->setChecked(true);
    } else if(zoomWidget->gridColor()==QColor(Qt::white)) {
        actionGridWhite->setChecked(true);
    } else if(zoomWidget->gridColor()==QColor()) {
        actionGridNone->setChecked(true);
    } 
}
void MainWindow::writeSettings()
{
    QSettings settings("NikoSams", "PixelReiter");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.setValue("selWinPos", selWin->pos());
    settings.setValue("zoomFactor", zoomWidget->zoomFactor());
    settings.setValue("updateInterval", zoomWidget->updateInterval());
    settings.setValue("gridColor", zoomWidget->gridColor());
    
}
    
void MainWindow::closeEvent(QCloseEvent */*event*/)
{
    writeSettings();
    delete selWin;
}


void MainWindow::exportCurrent()
{
    QString s = QFileDialog::getSaveFileName(
                this,
                tr("export current view"),
                QString(),
                "Images (*.png *.xpm *.jpg)");
    QImage image(zoomWidget->size(), QImage::Format_RGB32);
    QPainter painter(&image);
    zoomWidget->doPainting(painter);
    image.save(s, "PNG"); //fixme: different file-formats!
}
void MainWindow::importScreenshot()
{
    QString s = QFileDialog::getOpenFileName(
                this,
                tr("import screenshot"),
                QString(),
                "Images (*.png *.xpm *.jpg *.gif)");
    QImage image(s);
    if(image.isNull()) 
    {
        QMessageBox::critical(this, tr("import screenshot"), tr("invalid file"));
        return;
    }
    zoomWidget->setPixmap(QPixmap::fromImage(image));
}
