#include "imageitem.h"
#include <QGraphicsItem>
#include <QPointF>
#include <QDebug>

ImageItem::ImageItem()
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setScale(1);
}


