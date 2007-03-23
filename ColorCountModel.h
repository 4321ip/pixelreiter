#ifndef COLORCOUNTMODEL_H
#define COLORCOUNTMODEL_H

#include <QStandardItemModel>
#include "ColorCount.h"
class ColorCountModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ColorCountModel(QObject *parent = 0);

    ~ColorCountModel();

    QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(const QRgb& color) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
    QColor color(int row) const;

public slots:
    void setColors(const QList<ColorCount>& colors);

private:
    QList<ColorCount> m_colors;
};

#endif
