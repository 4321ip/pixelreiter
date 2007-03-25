#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#import <QMainWindow>
#import <QHash>
#import <QRgb>
#import "ColorCount.h"
#import "ui/ui_MainWindow.h"
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

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent=0);

public slots:

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent ( QKeyEvent * event );
private slots:
    void toggleGrab(bool);
    void toggleShowGrid(QAction* action);
    void zoomIn();
    void zoomOut();
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


    
    ZoomWidget* zoomWidget;
    SelWin* selWin;

    
    QComboBox* intervalBox;

    ColorCountModel* m_colorCountModel;

};
#endif
