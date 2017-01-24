#ifndef TRANSFORMTHREAD_H
#define TRANSFORMTHREAD_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <vector>
#include <QMutex>
#include "transformcurveview.h"

class TransformThread : public QThread
{
  Q_OBJECT
public:
  TransformThread(QObject* parent, const std::vector<unsigned int>& transform_map, QImage& image, QImage& transformed_image);

  void run();

  QMutex image_mutex;
  unsigned int recorded_thread_count = 1;
  unsigned int requested_thread_count = 1;
signals:
  void transformedImageUpdated();

private:
  const std::vector<unsigned int>& transform_map;
  QImage& image;
  QImage& transformed_image;
};

#endif // TRANSFORMTHREAD_H
