#include "liveimageview.h"

LiveImageView::LiveImageView(QWidget *parent)
  : QGraphicsView(parent) {
  setScene(new QGraphicsScene());
  scene()->clear();
  graphics_item = new ImageGraphicsItem(image());
  scene()->addItem(graphics_item);
}

void LiveImageView::ImageGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  painter->save();
  painter->drawImage(0, 0, image);
  painter->restore();
}


void LiveImageView::UpdateContents() {
  scene()->setSceneRect(image_.rect());
  fitInView(graphics_item, Qt::KeepAspectRatio);
  scene()->update();
}
