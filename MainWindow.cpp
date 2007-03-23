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
    selWin = new SelWin;

    zoomWidget = new ZoomWidget(selWin);
    setCentralWidget(zoomWidget);

    createActions();
    createToolBars();
    createStatusBar();
    createDockWindows();
    createMenus();
    
    readSettings();
}
void MainWindow::toggleGrab(bool i) {
    if(i)
        zoomWidget->play();
    else
        zoomWidget->pause();
}

void MainWindow::toggleShowGrid(bool i) {
    zoomWidget->setShowGrid(i);
}

void MainWindow::createActions() {
    grabAct = new QAction(tr("&grab"), this);
    grabAct->setCheckable(true);
    grabAct->setShortcut(tr("Ctrl+G"));
    grabAct->setStatusTip(tr("update screenshot"));
    connect(grabAct, SIGNAL(triggered(bool)), this, SLOT(toggleGrab(bool)));

    showGridAct = new QAction(tr("show &grid"), this);
    showGridAct->setCheckable(true);
    showGridAct->setStatusTip(tr("show grid"));
    connect(showGridAct, SIGNAL(triggered(bool)), this, SLOT(toggleShowGrid(bool)));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("quit application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    exportCurrentAct = new QAction(tr("&Export..."), this);
    exportCurrentAct->setStatusTip(tr("export current view"));
    connect(exportCurrentAct, SIGNAL(triggered()), this, SLOT(exportCurrent()));

    importScreenshotAct = new QAction(tr("&Import..."), this);
    importScreenshotAct->setStatusTip(tr("import screenshot"));
    connect(importScreenshotAct, SIGNAL(triggered()), this, SLOT(importScreenshot()));
}
void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(grabAct);
    fileMenu->addSeparator();
    fileMenu->addAction(importScreenshotAct);
    fileMenu->addAction(exportCurrentAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);
    
    QMenu* settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(showGridAct);
    QAction* colorDockAction = colorsDock->toggleViewAction();
    colorDockAction->setText(tr("show &colors"));
    settingsMenu->addAction(colorDockAction);
}
void MainWindow::createToolBars() {
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("FileToolbar");
    fileToolBar->addAction(grabAct);
    
    zoomBox = new QComboBox;
    zoomBox->insertItem(0, "1", 1);
    zoomBox->insertItem(1, "2", 2);
    zoomBox->insertItem(2, "3", 3);
    zoomBox->insertItem(3, "4", 4);
    zoomBox->insertItem(4, "5", 5);
    zoomBox->insertItem(5, "7", 7);
    zoomBox->insertItem(6, "10", 10);
    zoomBox->insertItem(7, "15", 15);
    zoomBox->insertItem(8, "20", 20);
    
    fileToolBar->addSeparator();
    fileToolBar->addWidget(new QLabel("Zoom:"));
    fileToolBar->addWidget(zoomBox);
    connect(zoomBox, SIGNAL(activated(int)), this, SLOT(zoomChanged(int)));

    intervalBox = new QComboBox;
	intervalBox->insertItem(0, "30ms", 30);
	intervalBox->insertItem(1, "100ms", 100);
    intervalBox->insertItem(2, "500ms", 500);
    intervalBox->insertItem(3, "1s",   1000);
    intervalBox->insertItem(4, "2s",   2000);
    intervalBox->insertItem(5, "3s",   3000);
    intervalBox->insertItem(6, "4s",   4000);
    intervalBox->insertItem(7, "5s",   5000);
    intervalBox->insertItem(8, "7s",   7000);
    intervalBox->insertItem(9, "10s", 10000);

    fileToolBar->addSeparator();
    fileToolBar->addWidget(new QLabel("Update:"));
    fileToolBar->addWidget(intervalBox);
    connect(intervalBox, SIGNAL(activated(int)), this, SLOT(intervalChanged(int)));
}
void MainWindow::zoomChanged(int i) {
    zoomWidget->setZoomFactor(zoomBox->itemData(i).toInt());
    zoomWidget->grabPixmap();
    zoomWidget->update();
}
void MainWindow::intervalChanged(int i) {
    zoomWidget->setUpdateInterval(intervalBox->itemData(i).toInt());
}
void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
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
void MainWindow::createDockWindows() {
    colorsDock = new QDockWidget(tr("Colors"), this);
    colorsDock->setObjectName("colorsDock");
    colorsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_colorCountModel = new ColorCountModel;

    connect(zoomWidget, SIGNAL(colorsChanged(QList<ColorCount>)), m_colorCountModel, SLOT(setColors(QList<ColorCount>))); 
    connect(m_colorCountModel, SIGNAL(layoutChanged()), this, SLOT(colorsTableChanged()));
    
    colorsTable = new QTreeView();
    //colorsTable->setViewMode(QListView::IconMode);
    colorsTable->setModel(m_colorCountModel);
    //colorsTable->setShowGrid(false);
    colorsTable->setIndentation(0);
    colorsTable->setAlternatingRowColors(true);
    colorsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(colorsTable->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(colorClicked(QModelIndex)));
    connect(zoomWidget, SIGNAL(markColorChanged( const QColor& )), this, SLOT(changeColor( const QColor& )));
    
    QWidget* dockWidget = new QWidget;
    
    QVBoxLayout* dockLayout = new QVBoxLayout;
    dockWidget->setLayout(dockLayout);

    dockLayout->addWidget(colorsTable);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    dockLayout->addLayout(buttonLayout);
    
    buttonLayout->addStretch();
    
    QPushButton* copyButton = new QPushButton(tr("copy"));
    buttonLayout->addWidget(copyButton);
    connect(copyButton, SIGNAL(clicked()), this, SLOT(copyColor()));
    

    colorsDock->setWidget(dockWidget);
    addDockWidget(Qt::RightDockWidgetArea, colorsDock);

}

void MainWindow::copyColor() 
{
    QClipboard *clipboard = QApplication::clipboard();
    QColor c(m_colorCountModel->color(colorsTable->selectionModel()->currentIndex().row()));
    clipboard->setText(c.name());
}
void MainWindow::readSettings()
{
    QSettings settings("NikoSoft", "PixelReiter");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    restoreState(settings.value("state").toByteArray());
    resize(size);
    move(pos);
    selWin->move(settings.value("selWinPos", QPoint(0, 0)).toPoint());

    int i = zoomBox->findData(settings.value("zoomFactor", 10).toString());
    if(i!=-1) {
        zoomBox->setCurrentIndex(i);
        zoomWidget->setZoomFactor(settings.value("zoomFactor", 10).toInt());
    }

    i = intervalBox->findData(settings.value("updateInterval", 1000).toInt());
    if(i!=-1) {
        intervalBox->setCurrentIndex(i);
        zoomWidget->setUpdateInterval(settings.value("updateInterval", 1000).toInt());
    }

    zoomWidget->setShowGrid(settings.value("showGrid", true).toBool());
    showGridAct->setChecked(zoomWidget->showGrid());
    
}
void MainWindow::writeSettings()
{
    QSettings settings("NikoSoft", "PixelReiter");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.setValue("selWinPos", selWin->pos());
    settings.setValue("zoomFactor", zoomWidget->zoomFactor());
    settings.setValue("updateInterval", zoomWidget->updateInterval());
    settings.setValue("showGrid", zoomWidget->showGrid());
    
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
