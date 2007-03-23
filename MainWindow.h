#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#import <QMainWindow>
#import <QHash>
#import <QRgb>
#import "ColorCount.h"
class ZoomWidget;
class SelWin;
class QAction;
class QMenu;
class QToolBar;
class QTreeView;
class QDockWidget;
class QComboBox;
class ColorCountModel;
class QModelIndex;
class QTableView;
class QListView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent=0);

public slots:

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void toggleGrab(bool);
    void toggleShowGrid(bool);
    void zoomChanged(int);
    void intervalChanged(int);
    void colorClicked(const QModelIndex & index);
    void colorsTableChanged();
    void changeColor(const QColor& color);
    void copyColor();
    
    void exportCurrent();
    void importScreenshot();

private:
    void readSettings();
    void writeSettings();

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    
    ZoomWidget* zoomWidget;
    SelWin* selWin;

    QAction *grabAct;
    QAction *quitAct;
    QAction *showGridAct;
    QAction *exportCurrentAct;
    QAction *importScreenshotAct;
    
    QMenu *fileMenu;
    QToolBar *fileToolBar;
    QDockWidget *colorsDock;
    QComboBox* zoomBox;

    QTreeView* colorsTable;
    
    QComboBox* intervalBox;

    ColorCountModel* m_colorCountModel;

};
#endif
