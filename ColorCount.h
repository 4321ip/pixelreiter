#ifndef COLORCOUNT_H
#define COLORCOUNT_H
#include <QRgb>
class ColorCount {
public:
    ColorCount(QRgb, int);
    QRgb color;
    int count;
    bool operator <(const ColorCount &d) const;
};

#endif
