#include "SelWin.h"
#include <QMouseEvent>
#include <QPainter>

SelWin::SelWin (QWidget * parent) :
    QWidget (parent, Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint)
{
    setAutoFillBackground(true);
    setPalette(QPalette(QColor(0, 0, 0)));
    setCursor(Qt::SizeAllCursor);
}
void SelWin::resizeEvent(QResizeEvent * /*event*/) {
    setMask(QRegion(QRect(0, 0, width(), height())) - QRect(2, 2+20, width()-4, height()-4-20));
}

void SelWin::setSelectionSize(const QSize& size) {
    resize(size + QSize(4, 4+20));
}
QRect SelWin::getSelection() {
    return(QRect(x()+2, y()+2+20, width()-4, height()-4-20));
}
void SelWin::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}
void SelWin::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void SelWin::paintEvent(QPaintEvent * /*event*/) {
    QPainter painter(this);
    painter.drawText(QRect(0, 0, width(), 20), Qt::AlignCenter, "PixelReiter");
    painter.setPen(Qt::DashLine);
    painter.drawRect(0,0,width()-2,height()-2);
}
