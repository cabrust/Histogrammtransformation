#ifndef LIVEIMAGEVIEW_H
#define LIVEIMAGEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QResizeEvent>
#include <QImage>

class LiveImageView : public QGraphicsView
{
  Q_OBJECT
public:
  class ImageGraphicsItem : public QGraphicsItem {
    public:
      QRectF boundingRect() const { return image.rect(); }
      ImageGraphicsItem(QImage& image) : image(image) {}
      void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    private:
      QImage& image;
  };

  LiveImageView(QWidget* parent = nullptr);

  QImage& image() { return image_; }

  void UpdateContents();

  void resizeEvent(QResizeEvent* evt) { UpdateContents(); }

private:
  QImage image_;
  ImageGraphicsItem* graphics_item;
};

#endif // LIVEIMAGEVIEW_H
