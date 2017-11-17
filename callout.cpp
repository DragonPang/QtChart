#include "callout.h"

#include <QPainter>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QChart>

Callout::Callout(QChart *chart):
    QGraphicsItem(chart),
    chart(chart)
{

}

QRectF Callout::boundingRect() const
{
    QPointF anchor = mapFromParent(chart->mapToPosition(this->anchor));
    QRectF rect;
    rect.setLeft(qMin(this->rect.left(), anchor.x()));
    rect.setRight(qMax(this->rect.right(), anchor.x()));
    rect.setTop(qMin(this->rect.top(), anchor.y()));
    rect.setBottom(qMax(this->rect.bottom(), anchor.y()));

    return rect;
}

void Callout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);

    QPointF anchor = mapFromParent(chart->mapToPosition(this->anchor));
    if (!rect.contains(anchor)) {
        QPointF point1, point2;

        // establish the position of the anchor point in relation to rect
        bool above = anchor.y() <= rect.top();
        bool aboveCenter = anchor.y() > rect.top() && anchor.y() <= rect.center().y();
        bool belowCenter = anchor.y() > rect.center().y() && anchor.y() <= rect.bottom();
        bool below = anchor.y() > rect.bottom();

        bool onLeft = anchor.x() <= rect.left();
        bool leftOfCenter = anchor.x() > rect.left() && anchor.x() <= rect.center().x();
        bool rightOfCenter = anchor.x() > rect.center().x() && anchor.x() <= rect.right();
        bool onRight = anchor.x() > rect.right();

        // get the nearest rect corner.
        qreal x = (onRight + rightOfCenter) * rect.width();
        qreal y = (below + belowCenter) * rect.height();
        bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
        bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

        qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
        qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);;
        point1.setX(x1);
        point1.setY(y1);

        qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);;
        qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10);;
        point2.setX(x2);
        point2.setY(y2);

        path.moveTo(point1);
        path.lineTo(anchor);
        path.lineTo(point2);
        path = path.simplified();
    }

    painter->setBrush(QColor(255, 255, 255));
    painter->drawPath(path);
    painter->drawText(textRect, text);
}

void Callout::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(true);
}

void Callout::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton){
        setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
        event->setAccepted(true);
    } else {
        event->setAccepted(false);
    }
}

void Callout::setText(const QString &text)
{
    this->text = text;
    QFontMetrics metrics(font);
    textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, text);
    textRect.translate(5, 5);
    prepareGeometryChange();
    rect = textRect.adjusted(-5, -5, 5, 5);
}

void Callout::setAnchor(QPointF point)
{
    anchor = point;
}

void Callout::updateGeometry()
{
    prepareGeometryChange();
    setPos(chart->mapToPosition(anchor) + QPoint(10, -50));
}
