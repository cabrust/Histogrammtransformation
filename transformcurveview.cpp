#include <vector>
#include <QMouseEvent>
#include <QGraphicsScene>
#include "transformcurveview.h"
#include "mainwindow.h"

TransformCurveView::TransformCurveView(QWidget* parent) : QGraphicsView(parent)
{
  transform_map_.resize(256);
  setScene(new QGraphicsScene());

  scene()->clear();
  scene()->setSceneRect(0, 0, 256, 256);
  scene()->addItem(new TransformGraphicsItem(transform_map_));
}

void TransformCurveView::handleMouseEvent(QMouseEvent *mouse_event) {
  int x_pos = mapToScene(mouse_event->pos()).x();
  int y_pos = mapToScene(mouse_event->pos()).y();
  y_pos = 255-y_pos;
  for(int x_pos_ = x_pos - 5; x_pos_ < x_pos + 5; x_pos_++) {
    if(x_pos_ > 0 && x_pos_ < 256 && y_pos > 0 && y_pos < 256) {
      transform_map_[x_pos_] = y_pos;
    }
  }
  innerTransformChanged();
}

void TransformCurveView::SetTransform(unsigned int c, unsigned int value) {
  if(transform_map_[c] != value) {
    transform_map_[c] = value;
    innerTransformChanged();
  }
}

void TransformCurveView::innerTransformChanged() {
  if(send_events) {
    scene()->update(0, 0, 256, 256);
    emit transformChanged();
  }
}

void TransformCurveView::mouseMoveEvent(QMouseEvent *mouse_event) {
  handleMouseEvent(mouse_event);
}

void TransformCurveView::mousePressEvent(QMouseEvent *evt) {
  handleMouseEvent(evt);
  evt->accept();
}

void TransformCurveView::TransformGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  painter->save();
  for(int c = 0; c < 256; c++) {
      painter->drawPoint(c, 255-transform_map[c]);
  }
  painter->restore();
}
