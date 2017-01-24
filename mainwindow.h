#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <chrono>

#include "transformthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void UpdateHistogramDisplays();

private slots:
    void on_action_ffnen_triggered();
    void on_transformView_transformChanged();

    void onTransformedImageUpdated();

    void on_resetButton_clicked();

    void on_gammaTransform_clicked();

    void on_dial_valueChanged(int value);

private:
    void ResetTransform();

    Ui::MainWindow *ui;
    TransformThread* transform_thread;

    QGraphicsScene* hist_scene;
    QGraphicsScene* transformed_hist_scene;

    std::vector<unsigned int> hist_gray;
    std::vector<unsigned int> transformed_hist_gray;

    std::chrono::system_clock::time_point last_frame = std::chrono::system_clock::now();
    double last_time_per_image = 0.2;
    double last_time_per_image_single = 0.2;
};

#endif // MAINWINDOW_H
