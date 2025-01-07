#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaDevices>
#include <QStandardPaths>
#include <QDir>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QPainter>

struct BoundingBox {
    QRect rect;  // Rectangle for the bounding box
    QString label; // Label for the object
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_camera(nullptr)
    , m_imageCapture(nullptr)
    , m_videoWidget(new QVideoWidget(this))
    , m_captureSession(new QMediaCaptureSession(this))
{
    ui->setupUi(this);

    // Ensure the image label scales properly
    ui->imageLabel->setScaledContents(true);
    ui->imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Set up the video display in the camera page
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_videoWidget);
    ui->videoFrame->setLayout(layout);

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cam : cameras) {
        ui->cameraComboBox->addItem(cam.description());
    }

    connect(ui->cameraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCameraSelected);
    connect(ui->captureButton, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    connect(ui->recaptureButton, &QPushButton::clicked, this, &MainWindow::onRecaptureButtonClicked);

    if (!cameras.isEmpty()) {
        onCameraSelected(0);
    }

    // Start on camera page (index 0)
    ui->stackedWidget->setCurrentIndex(0);
    qDebug() << "Application started on camera page. Current index:" << ui->stackedWidget->currentIndex();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_camera) {
        m_camera->stop();
        delete m_camera;
    }
}

void MainWindow::onCameraSelected(int index)
{
    qDebug() << "Camera selected. Index:" << index;
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (index < 0 || index >= cameras.size()) {
        qDebug() << "Invalid camera index:" << index;
        return;
    }

    if (m_camera) {
        qDebug() << "Stopping and deleting previous camera.";
        m_camera->stop();
        delete m_camera;
        m_camera = nullptr;
    }

    QCameraDevice device = cameras.at(index);
    qDebug() << "Setting up camera:" << device.description();
    m_camera = new QCamera(device, this);

    if (m_imageCapture) {
        delete m_imageCapture;
        m_imageCapture = nullptr;
    }

    m_imageCapture = new QImageCapture(this);

    m_captureSession->setCamera(m_camera);
    m_captureSession->setImageCapture(m_imageCapture);
    m_captureSession->setVideoOutput(m_videoWidget);

    connect(m_imageCapture, &QImageCapture::imageCaptured, this, &MainWindow::onImageCaptured);

    m_camera->start();
    qDebug() << "Camera started.";
}

void MainWindow::onCaptureButtonClicked()
{
    qDebug() << "Capture button clicked.";
    if (m_imageCapture && m_imageCapture->isReadyForCapture()) {
        qDebug() << "ImageCapture is ready. Capturing image...";
        m_imageCapture->capture();
        qDebug() << "Switching to loading page.";
        ui->stackedWidget->setCurrentIndex(1); // Loading page
        qDebug() << "Current page index:" << ui->stackedWidget->currentIndex();
    } else {
        qDebug() << "ImageCapture is not ready for capture.";
    }
}

void MainWindow::onImageCaptured(int id, const QImage &image)
{
    Q_UNUSED(id);
    qDebug() << "Image captured signal received.";
    qDebug() << "Captured image size:" << image.size();

    if (image.isNull()) {
        qDebug() << "Captured image is null. Check camera support for still images.";
        return;
    }

    m_capturedImage = image;

    // Convert to grayscale
    QImage grayImage = m_capturedImage.convertToFormat(QImage::Format_Grayscale8);

    // Detect face (very basic intensity-based method for demonstration)
    QRect detectedFace = detectFace(grayImage);

    // Draw bounding box around the detected face
    QImage imageWithBoxes = m_capturedImage.copy();
    QPainter painter(&imageWithBoxes);
    painter.setPen(QPen(Qt::red, 3));
    painter.setFont(QFont("Arial", 12));

    if (!detectedFace.isNull()) {
        painter.drawRect(detectedFace);
        painter.drawText(detectedFace.topLeft() + QPoint(5, -5), "Face");
    } else {
        qDebug() << "No face detected.";
    }

    painter.end();

    // Save the image with bounding boxes
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (savePath.isEmpty()) {
        savePath = QDir::currentPath(); // Fallback to current directory
    }

    QString fileName = savePath + "/FaceDetectedImage_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".jpg";
    if (imageWithBoxes.save(fileName)) {
        qDebug() << "Image with bounding boxes saved successfully at:" << fileName;
    } else {
        qDebug() << "Failed to save the image with bounding boxes.";
    }

    // Display the image with bounding boxes
    QTimer::singleShot(1000, [this, imageWithBoxes]() {
        qDebug() << "Processing done. Displaying result page with bounding boxes.";
        QPixmap pix = QPixmap::fromImage(imageWithBoxes);
        pix = pix.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->imageLabel->setPixmap(pix);
        ui->stackedWidget->setCurrentIndex(2); // Result page
    });
}

QRect MainWindow::detectFace(const QImage &grayImage)
{
    const int width = grayImage.width();
    const int height = grayImage.height();
    const uchar *pixels = grayImage.constBits();
    const int stride = grayImage.bytesPerLine();

    int faceLeft = -1, faceTop = -1, faceRight = -1, faceBottom = -1;

    // Focus on central region of the image (50% of the width and height)
    const int centerX = width / 2;
    const int centerY = height / 2;
    const int regionWidth = width / 2;
    const int regionHeight = height / 2;

    const int startX = centerX - regionWidth / 2;
    const int startY = centerY - regionHeight / 2;
    const int endX = centerX + regionWidth / 2;
    const int endY = centerY + regionHeight / 2;

    // Thresholds for detecting faces
    const int minBrightness = 180; // Minimum brightness for facial regions
    const int minSize = 50;        // Minimum width/height of the face

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            uchar brightness = pixels[y * stride + x];
            if (brightness > minBrightness) {
                if (faceLeft == -1 || x < faceLeft) faceLeft = x;
                if (faceRight == -1 || x > faceRight) faceRight = x;
                if (faceTop == -1 || y < faceTop) faceTop = y;
                if (faceBottom == -1 || y > faceBottom) faceBottom = y;
            }
        }
    }

    // Validate face dimensions
    if (faceRight - faceLeft >= minSize && faceBottom - faceTop >= minSize) {
        return QRect(QPoint(faceLeft, faceTop), QPoint(faceRight, faceBottom));
    }

    return QRect(); // Return an invalid QRect if no face is detected
}


void MainWindow::onRecaptureButtonClicked()
{
    qDebug() << "Recapture button clicked. Returning to camera page.";
    ui->stackedWidget->setCurrentIndex(0);
    qDebug() << "Current page index:" << ui->stackedWidget->currentIndex();
}

