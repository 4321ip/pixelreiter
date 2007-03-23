#include "ColorCount.h"
ColorCount::ColorCount(QRgb col, int c)
    : color(col), count(c)
{
}

bool ColorCount::operator <(const ColorCount &c) const {
    return count > c.count;
}

