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
enum Tool { ToolNone, ToolDraw, ToolColorPicker };

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
    QColor gridColor()  { return m_gridColor; }
    Tool currentTool() { return m_currentTool; }

    void doPainting(QPainter& painter);

public slots:
    void grabPixmap();
    void setZoomFactor(int factor);
    void setUpdateInterval(int i);
    void setMarkColor(QColor i) {
        m_markColor = i;
        update();
    }
    void setGridColor(QColor i) { m_gridColor = i; update(); }
    void setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; update(); }
    void setCurrentTool(Tool tool) { m_currentTool = tool; }

signals:
    void colorsChanged(const QList<ColorCount>& colors);
    void markColorChanged(const QColor& color);

private:
    QPixmap m_pixmap;
    int m_zoomFactor;
    static const int rulerWidth=20;
    QColor m_markColor;
    QColor m_gridColor;

    bool hasCurrentLine;
    int currentLine;
    QList<Line*> lines;
    
    void updateCursor( QMouseEvent * event );
    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage &image, const QSize &newSize);
    SelWin* selWin;
    QTimer* timer;
    Tool m_currentTool;
    bool m_isDrawing;
    QPoint m_drawingLastPoint;
    QImage m_drawingImage;

protected:
    void paintEvent(QPaintEvent * event);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void resizeEvent(QResizeEvent * event);
    void closeEvent(QCloseEvent* event);
};


#endif
