#include "ZoomWidget.h"
#include <QPainter>
#include <QTimer>
#include <QApplication>
#include <QPixmap>
#include <QDesktopWidget>
#include <QFont>
#include <QMouseEvent>
#include <QDebug>
#include "SelWin.h"
#include <QImage>
#include <QHash>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QIcon>
#include "ColorCount.h"
ZoomWidget::ZoomWidget(SelWin* sw, QWidget* parent) : QWidget(parent), selWin(sw)
{
    //setAttribute(Qt::WA_DeleteOnClose);

    setMouseTracking(true);
    hasCurrentLine = false;
    m_zoomFactor = 10;
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(grabPixmap()));
}
void ZoomWidget::closeEvent(QCloseEvent* /*event*/)
{
}

void ZoomWidget::pause() {
    selWin->hide();
    timer->stop();
}
void ZoomWidget::play() {
    selWin->show();
    grabPixmap();
    timer->start();
}


ZoomWidget::~ZoomWidget() {
}

void ZoomWidget::grabPixmap()
{
    if(!selWin->isVisible()) return;
    int grabWidth = (width()-rulerWidth)/m_zoomFactor +1;
    int grabHeight = (height()-rulerWidth)/m_zoomFactor +1;
    
    m_pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), selWin->getSelection().x(), selWin->getSelection().y(), grabWidth, grabHeight);

	QHash<QRgb, int> pixels;
    QImage image = m_pixmap.toImage();
    for(int x=0;x<grabWidth;x++) {
        for(int y=0;y<grabHeight;y++) {
            QRgb pixel = image.pixel(x, y);
            pixels[pixel]++;
            
        }
    }

    QList<ColorCount> colors;
    QHashIterator<QRgb, int> i(pixels);
    while (i.hasNext()) {
        i.next();
        colors.append(ColorCount(i.key(), i.value()));
    }
    qSort(colors);

    emit colorsChanged(colors);

    update();
}


void ZoomWidget::paintEvent(QPaintEvent * /*event*/) {
    QPainter painter(this);
    doPainting(painter);
}

void ZoomWidget::doPainting(QPainter& painter)
{

    //paint the screenshot
    if(!m_pixmap.isNull()) {
        QPixmap scaled = m_pixmap.scaled(m_pixmap.size()*m_zoomFactor);
        painter.drawPixmap(rulerWidth, rulerWidth, scaled);
    }

    //mark active pixels
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(255, 0, 0));
    painter.setPen(pen);

    if(m_markColor.isValid())
    {
        QImage image = m_pixmap.toImage();
        for(int x=0;x<m_pixmap.size().width();x++) {
            for(int y=0;y<m_pixmap.size().height();y++) {
                if(image.pixel(x, y)==m_markColor.rgb()) {
                    painter.drawRect(rulerWidth+x*m_zoomFactor, rulerWidth+y*m_zoomFactor, m_zoomFactor, m_zoomFactor);
                }
            }
        }
    }
    
    //draw grid
    if(m_showGrid)
    {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(QColor(255, 255, 255, 200));
        painter.setPen(pen); 
        static const int gridSize=10;
        for(int x=rulerWidth;x<width();x+=gridSize*m_zoomFactor) {
            painter.drawLine(x, rulerWidth, x, height()-rulerWidth);
        }
        for(int y=rulerWidth;y<height();y+=gridSize*m_zoomFactor) {
            painter.drawLine(rulerWidth, y, width()-rulerWidth, y);
        }
    }

    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(0, 0, 0));
    painter.setPen(pen);
    
    //draw the rulers:
    painter.fillRect (0, 0, width(), rulerWidth, QBrush(QColor(255, 255, 255)));
    painter.fillRect (0, 0, rulerWidth, height(), QBrush(QColor(255, 255, 255)));

    //draw the ruler ticks
    QFont font;
    font.setPointSize(6);
    painter.setFont(font);
    for(int i=0;i<(width()-rulerWidth)/(20);i++) {
        int x = i*20 + rulerWidth;
        if(i%2==0) {
            painter.drawLine(x, rulerWidth-8, x, rulerWidth);
            painter.drawText(QRect(x-9, 2, 18, 10), Qt::AlignCenter, QString("%1").arg(i*20/m_zoomFactor));
        } else {
            painter.drawLine(x, rulerWidth-5, x, rulerWidth);
        }
    }
    for(int i=0;i<(height()-rulerWidth)/(20);i++) {
        int y = i*20 + rulerWidth;
        if(i%2==0) {
            painter.drawLine(rulerWidth-8, y, rulerWidth, y);
            painter.drawText(QRect(2, y-9, 10, 18), Qt::AlignCenter, QString("%1").arg(i*20/m_zoomFactor));
        } else {
            painter.drawLine(rulerWidth-5, y, rulerWidth, y);
        }
    }
    
    //draw the lines
    QList<int> posX;
    QList<int> posY;
    for(int i=0;i<lines.count();i++)
    {
        if(hasCurrentLine && i==currentLine) {
            QPen pen;
            pen.setStyle(Qt::DashLine);
            pen.setColor(QColor(255, 0, 0));
            painter.setPen(pen);
        } else {
            QPen pen;
            pen.setStyle(Qt::SolidLine);
            pen.setColor(QColor(0, 0, 255));
            painter.setPen(pen);
        }
        Line* line = lines.at(i);
        if(line->orientation == Qt::Horizontal) {
            painter.drawLine(line->position*m_zoomFactor + rulerWidth, rulerWidth, line->position*m_zoomFactor + rulerWidth, height());
            posX << line->position;
        } else if (line->orientation == Qt::Vertical) {
            painter.drawLine(rulerWidth, line->position*m_zoomFactor + rulerWidth, width(), line->position*m_zoomFactor + rulerWidth);
            posY << line->position;
        }
    }
    //on the edgre make 30px lighter
    painter.fillRect(rulerWidth, height()-30, width(), 30, QBrush(QColor(255, 255, 255, 200)));
    painter.fillRect(width()-30, rulerWidth, 30, height()-rulerWidth-30, QBrush(QColor(255, 255, 255, 200)));
    
    //array for the pointer <--->
    static const QPoint arrowPoints[3] = {
        QPoint(0, 0),
        QPoint(8, 4),
        QPoint(0, 8)
    };

    //measure the number of px between the lines (x)
    qSort(posX);
    font.setPointSize(8);
    painter.setFont(font);
    painter.setPen(QColor(0, 0, 0));
    int last = 0;
    foreach(int x, posX)
    {
        painter.drawLine(last*m_zoomFactor + rulerWidth, height()-10, x*m_zoomFactor + rulerWidth, height()-10);
        painter.drawText(QRect(last*m_zoomFactor + rulerWidth, height()-30, (x-last)*m_zoomFactor, 20),
                Qt::AlignCenter | Qt::AlignBottom, QString("%1").arg(x-last));

        bool arrowOnOutside = false;
        if((x-last)*m_zoomFactor < 40) {
            qSwap(x, last);
            arrowOnOutside = true;
        }

        painter.save();
        //arrow right
        painter.setBrush(QBrush(QColor(0, 0, 0)));
        painter.translate(x*m_zoomFactor + rulerWidth-8, height()-10 -4);
        painter.drawPolygon(arrowPoints, 3, Qt::WindingFill);
        
        //arrow left
        painter.translate((last-x)*m_zoomFactor+16, 8);
        painter.rotate(180);
        painter.drawPolygon(arrowPoints, 3);
        painter.restore();

        if(!arrowOnOutside) //else qSwaped allready
            last = x;
    }
    
    //measure the number of px between the lines (y)
    qSort(posY);
    last = 0;
    foreach(int y, posY) {
        painter.drawLine(width()-10, last*m_zoomFactor + rulerWidth, width()-10, y*m_zoomFactor + rulerWidth);
        painter.drawText(QRect(width()-45, last*m_zoomFactor + rulerWidth, 30, (y-last)*m_zoomFactor),
                Qt::AlignRight | Qt::AlignVCenter, QString("%1").arg(y-last));

        bool arrowOnOutside = false;
        if((y-last)*m_zoomFactor < 40) {
            qSwap(y, last);
            arrowOnOutside = true;
        }

        painter.save();
        //arrow right
        painter.setBrush(QBrush(QColor(0, 0, 0)));
        painter.translate(width()-6, y*m_zoomFactor + rulerWidth-8);
        painter.rotate(90);
        painter.drawPolygon(arrowPoints, 3, Qt::WindingFill);
        
        //arrow left
        painter.rotate(-90);
        painter.translate(-8, (last-y)*m_zoomFactor+16);
        painter.rotate(-90);
        painter.drawPolygon(arrowPoints, 3);
        painter.restore();

        if(!arrowOnOutside) //else qSwaped allready
            last = y;
    }
}
void ZoomWidget::updateCursor( QMouseEvent * event ) {
    if(hasCurrentLine) {
        Line* line = lines.at(currentLine);
        if(line->orientation == Qt::Horizontal) {
            setCursor(Qt::SizeHorCursor);
        } else if (line->orientation == Qt::Vertical) {
            setCursor(Qt::SizeVerCursor);
        }
        return;
    }
    for(int i=0;i<lines.count();i++)
    {
        Line* line = lines.at(i);
        if(line->orientation == Qt::Horizontal) {
            int lineX = line->position * m_zoomFactor + rulerWidth;
            if(event->pos().x()-2 < lineX && event->pos().x()+2 > lineX) {
                setCursor(Qt::SizeHorCursor);
                return;
            }
        } else if (line->orientation == Qt::Vertical) {
            int lineY = line->position * m_zoomFactor + rulerWidth;
            if(event->pos().y()-2 < lineY && event->pos().y()+2 > lineY) {
                setCursor(Qt::SizeVerCursor);
                return;
            }
        }
    }
    unsetCursor();
}

void ZoomWidget::mouseMoveEvent ( QMouseEvent * event ) {
    if(hasCurrentLine)
    {
        Line* line = lines.at(currentLine);
        if(line->orientation == Qt::Vertical) {
            int y = event->pos().y();
            if(y >= height()) y = height();
            y = y - rulerWidth;
            y = y / m_zoomFactor;
            if(y < 0) y = 0;
            line->position = y;
        } else if (line->orientation == Qt::Horizontal) {
            int x = event->pos().x();
            if(x >= width()) x = width();
            x = x - rulerWidth;
            x = x / m_zoomFactor;
            if(x < 0) x = 0;
            line->position = x;
        }
        update();
    }
    updateCursor(event);
}

void ZoomWidget::mousePressEvent ( QMouseEvent * event )
{
    if(event->button() == Qt::LeftButton)
    {
        
        if(event->pos().x() < rulerWidth && event->pos().y() > rulerWidth) {
            Line* line = new Line;
            line->position = 0;
            line->orientation = Qt::Horizontal;
            currentLine = lines.count();
            lines.insert(currentLine, line);
            hasCurrentLine = true;
            updateCursor(event);
            update();
        } else if (event->pos().x() > rulerWidth && event->pos().y() < rulerWidth) {
            Line* line = new Line;
            line->position = 0;
            line->orientation = Qt::Vertical;
            currentLine = lines.count();
            lines.insert(currentLine, line);
            hasCurrentLine = true;
            updateCursor(event);
            update();
        } else {
            for(int i=0;i<lines.count();i++)
            {
                Line* line = lines.at(i);
                if(line->orientation == Qt::Horizontal) {
                    int lineX = line->position * m_zoomFactor + rulerWidth;
                    if(event->pos().x()-2 < lineX && event->pos().x()+2 > lineX) {
                        currentLine = i;
                        hasCurrentLine = true;
                        updateCursor(event);
                        update();
                        break;
                    }
                } else if (line->orientation == Qt::Vertical) {
                    int lineY = line->position * m_zoomFactor + rulerWidth;
                    if(event->pos().y()-2 < lineY && event->pos().y()+2 > lineY) {
                        currentLine = i;
                        hasCurrentLine = true;
                        updateCursor(event);
                        update();
                        break;
                    }
                }
            }
        }
    }
}

void ZoomWidget::mouseReleaseEvent ( QMouseEvent * event )
{
    if(hasCurrentLine && event->button() == Qt::LeftButton)
    {
        Line* line = lines.at(currentLine);
        if(line->position==0) {
            //if line is at position 0 delete it again
            lines.removeAt(currentLine);
        }

        hasCurrentLine = false;
        unsetCursor();
        update();
    }
    else if(event->button() == Qt::LeftButton)
    {
        if(event->pos().x() > rulerWidth && event->pos().y() > rulerWidth) 
        {
            if(event->pos().x() > m_pixmap.size().width() || event->pos().y() > m_pixmap.size().height() ) {
                QPoint imagePos = ((event->pos()) - QPoint(rulerWidth, rulerWidth)) / m_zoomFactor;
                QImage image = m_pixmap.toImage();
                m_markColor = image.pixel(imagePos.x(), imagePos.y());
                emit markColorChanged(m_markColor);
                update();
            }

        }
    }
}

void ZoomWidget::resizeEvent(QResizeEvent * event) {
    selWin->setSelectionSize(event->size()/m_zoomFactor);
	grabPixmap();
	update();
}

void ZoomWidget::setZoomFactor(int factor)
{
    m_zoomFactor = factor;
    selWin->setSelectionSize(size()/m_zoomFactor);
}

void ZoomWidget::setUpdateInterval(int i) {
    timer->setInterval(i);
}
int ZoomWidget::updateInterval() {
    return timer->interval();
}
