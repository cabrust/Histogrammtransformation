#ifndef TRANSFORMCURVEVIEW_H
#define TRANSFORMCURVEVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <vector>

class TransformCurveView : public QGraphicsView
{
  Q_OBJECT
public:
  class TransformGraphicsItem : public QGraphicsItem {
    public:
      QRectF boundingRect() const { return QRectF(0,0,256,256); }
      TransformGraphicsItem(std::vector<unsigned int>& transform_map) : transform_map(transform_map) {}
      void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    private:
      std::vector<unsigned int>& transform_map;
  };

  explicit TransformCurveView(QWidget* parent);

  unsigned int GetTransform(unsigned int c) const { return transform_map_[c]; }
  void SetTransform(unsigned int c, unsigned int value);
  void SetSendEvents(bool send_events) { this->send_events = send_events; if(send_events) innerTransformChanged();}

  const std::vector<unsigned int>& transform_map() const { return transform_map_; }
signals:
  void transformChanged();

protected:
  void mousePressEvent(QMouseEvent* evt);
  void mouseMoveEvent(QMouseEvent* evt);

private:
  void innerTransformChanged();
  void handleMouseEvent(QMouseEvent* evt);

  bool send_events = true;

  std::vector<unsigned int> transform_map_;
};

#endif // TRANSFORMCURVEVIEW_H
