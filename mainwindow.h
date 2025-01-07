#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QRect>

class QCamera;
class QImageCapture;
class QVideoWidget;
class QMediaCaptureSession;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCameraSelected(int index);
    void onCaptureButtonClicked();
    void onImageCaptured(int id, const QImage &image);
    void onRecaptureButtonClicked();

private:
    QRect detectFace(const QImage &grayImage); // Add this declaration

    Ui::MainWindow *ui;

    QCamera *m_camera;
    QImageCapture *m_imageCapture;
    QVideoWidget *m_videoWidget;
    QImage m_capturedImage;
    QMediaCaptureSession *m_captureSession;
};

#endif // MAINWINDOW_H
