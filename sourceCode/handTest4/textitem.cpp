#include "textitem.h"

TextItem::TextItem(QString text)
    :QGraphicsTextItem(text)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setScale(1);
}
