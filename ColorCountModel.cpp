#include "ColorCountModel.h"
#include <QColor>
#include <QDebug>
#include <QPixmap>
#include <QIcon>

ColorCountModel::ColorCountModel(QObject *parent)
 : QAbstractItemModel(parent)
{
}

ColorCountModel::~ColorCountModel()
{
}
QModelIndex ColorCountModel::index(int row, int column,
                              const QModelIndex &parent) const
{
    if(parent==QModelIndex()) {
        return createIndex(row, column, 0);
    } else {
        return QModelIndex();
    }
}
QModelIndex ColorCountModel::index(const QRgb& color) const
{
    for(int i=0;i<m_colors.size();i++)
    {
        if(m_colors.at(i).color == color) {
            return index(i, 0);
        }
    }
    return QModelIndex();
}

QModelIndex ColorCountModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

void ColorCountModel::setColors(const QList<ColorCount>& colors)
{
    //beginInsertRows(QModelIndex(), 0, colors.count()-m_colors.count());
    m_colors = colors;
    //endInsertRows();
    emit layoutChanged();
    
    //emit dataChanged(index(0, 0), index(m_colors.count(), 2));
}

int ColorCountModel::rowCount(const QModelIndex &parent) const
{
    if(parent==QModelIndex()) {
        return m_colors.size();
    } else
        return 0;
}

int ColorCountModel::columnCount(const QModelIndex &parent) const
{
    if(parent==QModelIndex())
        return 2;
    else
        return 0;
}

QVariant ColorCountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_colors.size())
        return QVariant();

    if (index.column() >= 2)
        return QVariant();

    if (role == Qt::DisplayRole) {
        if(index.column()==0) {
            return QColor(m_colors.at(index.row()).color).name();
        } else if(index.column()==1) {
            return m_colors.at(index.row()).count;
        } else {
            return QVariant();
        }
        
    } else if(role==Qt::DecorationRole && index.column()==0) {
        QColor color(m_colors.at(index.row()).color);
        QPixmap iconPixmap(16, 16);
        iconPixmap.fill(color);
        QIcon icon(iconPixmap);
        return icon;
    } else
        return QVariant();

}
QColor ColorCountModel::color(int row) const
{
    if(row >= m_colors.count()) return QColor();
    return m_colors.at(row).color;
}
QVariant ColorCountModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    if (role == Qt::DisplayRole) {
        if(orientation==Qt::Horizontal) {
            if(section==0) {
                return tr("Color");
            } else if(section==1) {
                return tr("Count");
            } else {
                return QVariant();
            }
        } else {
            return QString(section+1);
        }
    } else
        return QVariant();
}

