#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QRgb>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <omp.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hist_scene = new QGraphicsScene(this);
    transformed_hist_scene = new QGraphicsScene(this);

    ui->histogramView->setScene(hist_scene);

    ui->transformedHistogramView->setScene(transformed_hist_scene);
    hist_gray.resize(256);
    transformed_hist_gray.resize(256);

    ResetTransform();

    omp_set_num_threads(1);
    transform_thread = new TransformThread(this, ui->transformView->transform_map(), ui->imageView->image(), ui->transformedImageView->image());
    QObject::connect(transform_thread, SIGNAL(transformedImageUpdated()), this, SLOT(onTransformedImageUpdated()));
    transform_thread->start();

}

MainWindow::~MainWindow()
{
    delete transform_thread;
    delete ui;
}

void MainWindow::onTransformedImageUpdated() {
  // Zeitdauer seit letzem Update messen
  std::chrono::duration<double> t_diff = std::chrono::system_clock::now() - last_frame;

  ui->transformedImageView->UpdateContents();

  // Zeitdauer ausgeben
  const double moving_average_coefficient = 0.95;
  double time_per_image = moving_average_coefficient * last_time_per_image + (1.0 - moving_average_coefficient) * t_diff.count();
  last_time_per_image = time_per_image;

  // Zeit für einzelnen Thread speicher
  int thread_count = transform_thread->recorded_thread_count;
  if(thread_count == 1) {
    last_time_per_image_single = last_time_per_image;
  }

  std::stringstream ss;
  ss << std::setprecision(3) << time_per_image << "s" << std::endl << std::setprecision(3) << 1.0 / time_per_image << " FPS";
  if(thread_count > 1) {
    ss << " (" << std::setprecision(2) << last_time_per_image_single / last_time_per_image << "x)";
  }
  ss << std::endl <<  "Verwendete Threads: " << transform_thread->recorded_thread_count;
  ui->fpsLabel->setText(QString::fromStdString(ss.str()));

  // Zeichnen absichtlich nicht mitzählen :)
  last_frame = std::chrono::system_clock::now();
}

void MainWindow::on_action_ffnen_triggered()
{
    // Bild laden
    QString fileName = QFileDialog::getOpenFileName(this, "Öffnen...",QString(),"JPEG-Bild (*.jpg *.jpeg)");
    transform_thread->image_mutex.lock();
    ui->imageView->image().load(fileName);
    ui->imageView->UpdateContents();
    transform_thread->image_mutex.unlock();

    // Histogramm berechnen
    for(int c = 0; c < 256; c++) {
        hist_gray[c] = 0;
    }

    QImage& image = ui->imageView->image();
    for(int y = 0; y < image.height(); y++) {
        for(int x = 0; x < image.width(); x++) {
            QColor pixel = QColor::fromRgb(image.pixel(x, y));
            int color_value = (pixel.red() + pixel.green() + pixel.blue()) / 3;
            hist_gray[color_value]++;
        }
    }
    UpdateHistogramDisplays();
    last_frame = std::chrono::system_clock::now();
}

void MainWindow::UpdateHistogramDisplays() {
    // Maximum berechnen
    unsigned int max = 0;
    for(int c = 0; c < 256; c++) {
        if(hist_gray[c] > max)
            max = hist_gray[c];
    }

    // Histogramm darstellen
    hist_scene->clear();
    hist_scene->setSceneRect(0,0,256,256);
    for(int c = 0; c < 256; c++) {
        float height = 255.0f * ((float)hist_gray[c]) / (float)max;
        hist_scene->addRect(c, 255-(int)height, 1, (int)height, QPen(), QBrush(QColor(0,0,0)));
    }

    // Transformation anwenden
    for(int c = 0; c < 256; c++) {
        transformed_hist_gray[c] = 0;
    }
    for(int c = 0; c < 256; c++) {
        transformed_hist_gray[ui->transformView->GetTransform(c)] += hist_gray[c];
    }

    // Maximum berechnen
    max = 0;
    for(int c = 0; c < 256; c++) {
        if(transformed_hist_gray[c] > max)
            max = transformed_hist_gray[c];
    }

    // Histogramm darstellen
    transformed_hist_scene->clear();
    transformed_hist_scene->setSceneRect(0,0,256,256);
    for(int c = 0; c < 256; c++) {
        float height = 255.0f * ((float)transformed_hist_gray[c]) / (float)max;
        transformed_hist_scene->addRect(c, 255-(int)height, 1, (int)height, QPen(), QBrush(QColor(0,0,0)));
    }
}

void MainWindow::ResetTransform() {
  ui->transformView->SetSendEvents(false);
  for(int c = 0; c < 256; c++) {
    ui->transformView->SetTransform(c, c);
  }
  ui->transformView->SetSendEvents(true);
}

void MainWindow::on_transformView_transformChanged()
{
  UpdateHistogramDisplays();
}

void MainWindow::on_resetButton_clicked()
{
  ResetTransform();
}

void MainWindow::on_gammaTransform_clicked()
{
  ui->transformView->SetSendEvents(false);
  for(int c = 0; c < 256; c++) {
    float val = c / 255.0f;
    float transformed_val = pow(val, 2.2);
    ui->transformView->SetTransform(c, (unsigned int)(transformed_val * 255));
  }
  ui->transformView->SetSendEvents(true);
}

void MainWindow::on_dial_valueChanged(int value)
{
  std::stringstream ss; ss << "Threads: " << value;
  ui->threadCountLabel->setText(QString::fromStdString(ss.str()));
  transform_thread->requested_thread_count = value;
}
