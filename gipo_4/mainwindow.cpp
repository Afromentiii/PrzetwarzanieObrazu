#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QFileDialog>


QImage last_change_image;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));

    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
    connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged2(int)));
    connect(ui->horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged3(int)));
    connect(ui->horizontalSlider_4, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged4(int)));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setReferenceImage(const QImage &image)
{
    reference_image = image;
}

void MainWindow::openImage()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("Images (*.png *.jpg *.bmp)"));

    if (!filename.isEmpty())
    {
        QImage image(filename);
        setReferenceImage(image);
        last_change_image = image;
        if (image.isNull())
        {
            qDebug() << "Failed to load image";
        }
        else
        {
            ui->image->setPixmap(QPixmap::fromImage(image));
            ui->image->resize(image.width()  ,image.height()   );

        }
    }
    else
    {
        qDebug() << "Image not loaded";
    }
}
int matrix[3][3] = {{1,0,0},
                    {0,1,0},
                    {0,0,1}};
int moveVector[3][1] = {{0},
                    {0},
                    {1}};

int result[3][1] = {{0},
                    {0},
                    {1}};

void MainWindow::multipleMatrix()
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 1; ++j) {
            for (int k = 0; k < 3; ++k) {
                result[i][j] += matrix[i][k] * moveVector[k][j];
            }
        }
    }
}

void MainWindow::setMatrixForRasterShift(int x, int y)
{
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[0][2] = ui->horizontalSlider->value();

    matrix[1][0] = 0;
    matrix[1][1] = 1;
    matrix[1][2] = ui->horizontalSlider_2->value();

    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 1;

    moveVector[0][0] = x;
    moveVector[1][0] = y;

    result[0][0] = 0;
    result[1][0] = 0;
    result[2][0] = 1;
}

void printMatrix()
{
    qDebug() << result[0][0];
    qDebug() << result[1][0];
}
void MainWindow::setMatrixForScaling(int x, int y)
{
    matrix[0][0] = ui->horizontalSlider_3->value();
    matrix[0][1] = 0;
    matrix[0][2] = 0;

    matrix[1][0] = 0;
    matrix[1][1] = ui->horizontalSlider_3->value();
    matrix[1][2] = 0;

    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 0;

    moveVector[0][0] = x;
    moveVector[1][0] = y;

    result[0][0] = 0;
    result[1][0] = 0;
    result[2][0] = 1;
}

void MainWindow::rasterImageShiftXY(QImage &src, QImage &dst, int v)
{
    ui->horizontalSlider->setMinimum(-reference_image.width());
    ui->horizontalSlider->setMaximum(reference_image.width());

    ui->horizontalSlider_2->setMinimum(-reference_image.height());
    ui->horizontalSlider_2->setMaximum(reference_image.height());

    dst.fill(Qt::white);
    for (int y = 0; y < src.height(); ++y)
        {
            for (int x = 0; x < src.width(); ++x)
            {
                setMatrixForRasterShift(x,y);
                multipleMatrix();
                if (result[0][0] < src.width() && result[1][0] < src.height() && result[0][0] >= 0 && result[1][0] >= 0)
                {
                        dst.setPixel(result[0][0], result[1][0], src.pixel(x, y));
                }
            }
        }
}

void MainWindow::sliderChanged(int v)
{
    if (!reference_image.isNull())
    {
        transformed_image = reference_image;
        rasterImageShiftXY(reference_image, transformed_image, v);
        ui->image->setPixmap(QPixmap::fromImage(transformed_image));
    }
}

void MainWindow::sliderChanged2(int v)
{
    if (!reference_image.isNull())
    {
        transformed_image = reference_image;
        rasterImageShiftXY(reference_image, transformed_image, v);
        ui->image->setPixmap(QPixmap::fromImage(transformed_image));
    }
}

void MainWindow::sliderChanged3(int v)
{
    if (!reference_image.isNull())
    {
        transformed_image = reference_image;
        scaleImage(reference_image, transformed_image, v);
    }
}

void MainWindow::sliderChanged4(int v)
{
    if (!reference_image.isNull())
    {
        transformed_image = reference_image;
        scaleImage(reference_image, transformed_image, v);
        ui->image->setPixmap(QPixmap::fromImage(transformed_image));
    }
}


void MainWindow::scaleImage(QImage &src, QImage &dst, int v)
{
    QImage image;


    int scaleValue = ui->horizontalSlider_3->value();
    if(scaleValue > 1)
    {
        ui->image->resize(src.width() * scaleValue, src.height() * scaleValue);
        image = QImage(src.width() * scaleValue, src.height() * scaleValue, QImage::Format_RGB888);

        for (int y = 0; y < src.height(); ++y)
        {
            for (int x = 0; x < src.width(); ++x)
            {
                setMatrixForScaling(x,y);
                multipleMatrix();
                int newX = result[0][0];
                int newY = result[1][0];

                if (newX >= 0 && newX < image.width() && newY >= 0 && newY < image.height())
                {
                    image.setPixel(newX, newY, src.pixel(x, y));
                }
            }
        }

        for (int y = 0; y < image.height(); ++y)
        {
            for (int x = 0; x < image.width(); ++x)
            {
                int originalX = x / scaleValue;
                int originalY = y / scaleValue;

                originalX = std::min(originalX, src.width() - 1);
                originalY = std::min(originalY, src.height() - 1);

                image.setPixel(x, y, src.pixel(originalX, originalY));
            }
        }

    }
    else if(scaleValue == 0 || scaleValue == 1)
    {
        ui->image->resize(src.width() * 1, src.height() * 1 );
        image = QImage(src.width() * 1, src.height() * 1, QImage::Format_RGB888);
        image = reference_image;
        ui->image->setPixmap(QPixmap::fromImage(image));
    }
    ui->image->setPixmap(QPixmap::fromImage(image));



}

