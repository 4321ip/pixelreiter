#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QList>
#include <QColor>
#include "ColorCount.h"
class SelWin;
class QTimer;

struct Line {
    int position;
    QFlags<Qt::Orientation> orientation;
};

class ZoomWidget : public QWidget
{
    Q_OBJECT
public:
    ZoomWidget(SelWin* sw, QWidget* parent=0);
    ~ZoomWidget();
    void pause();
    void play();
    int zoomFactor() { return m_zoomFactor; }
    int updateInterval();
    QColor markColor() { return m_markColor; }
    bool showGrid()  { return m_showGrid; }
    void doPainting(QPainter& painter);

public slots:
    void grabPixmap();
    void setZoomFactor(int factor);
    void setUpdateInterval(int i);
    void setMarkColor(QColor i) { m_markColor = i; update(); }
    void setShowGrid(bool i) { m_showGrid = i; update(); }
    void setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; update(); }

signals:
    void colorsChanged(const QList<ColorCount>& colors);
    void markColorChanged(const QColor& color);

private:
    void closeEvent(QCloseEvent* event);
    QPixmap m_pixmap;
    int m_zoomFactor;
    static const int rulerWidth=20;
    QColor m_markColor;
    bool m_showGrid;

    bool hasCurrentLine;
    int currentLine;
    QList<Line*> lines;
    
    void updateCursor( QMouseEvent * event );
    SelWin* selWin;
    QTimer* timer;
    
protected:
    void paintEvent(QPaintEvent * event);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void resizeEvent(QResizeEvent * event);
};


#endif
