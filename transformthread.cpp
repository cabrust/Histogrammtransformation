#include "transformthread.h"
#include <QDebug>
#include <QRgb>
#include <omp.h>

TransformThread::TransformThread(QObject* parent, const std::vector<unsigned int>& transform_map, QImage& image, QImage& transformed_image) : QThread(parent),
 transform_map(transform_map), image(image), transformed_image(transformed_image)
{

}

void TransformThread::run() {
  while(true) {
    // Bild sperren, damit es nicht verändert wird
    image_mutex.lock();

    // Prüfen, ob Bild vorhanden ist
    if(image.width() > 0 && image.height() > 0) {

      // Bild hat Größe geändert
      if(transformed_image.width() != image.width() ||
         transformed_image.height() != image.height()) {
        transformed_image = QImage(image.size(), image.format());
      }

      // Transformation durchführen
#pragma omp parallel num_threads(requested_thread_count) default(shared)
      {
        if(omp_get_thread_num() == 0) {
          recorded_thread_count = omp_get_num_threads();
        }
#pragma omp for
        for(int y = 0; y < image.height(); y++) {
          for(int x = 0; x < image.width(); x++) {
            QColor color(image.pixel(x, y));
            int hue, saturation, value, alpha;
            color.getHsv(&hue, &saturation, &value, &alpha);
            value = transform_map[value];
            color.setHsv(hue, saturation, value, alpha);
            transformed_image.setPixel(x, y, color.rgba());
          }
        }
      }

      // Bild wieder freigeben
      image_mutex.unlock();
      emit transformedImageUpdated();
    } else {
      // Bild wieder freigeben
      image_mutex.unlock();

      // Zeitscheibe des Threads freigeben für andere
      yieldCurrentThread();
    }
  }
}

