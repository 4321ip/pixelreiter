#ifndef SELWIN_H
#define SELWIN_H
#include <QWidget>
#include <QPoint>
class SelWin : public QWidget {
    Q_OBJECT
public:
    SelWin(QWidget* parent=0);
    void setSelectionSize(const QSize& size);
    QRect getSelection();
protected:
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent * event);
private:
    QPoint dragPosition;


};

#endif
