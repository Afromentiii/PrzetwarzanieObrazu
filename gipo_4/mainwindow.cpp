#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QFileDialog>


QImage loaded_image;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
    connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged2(int)));
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
        loaded_image = image;
        if (image.isNull())
        {
            qDebug() << "Failed to load image";
        }
        else
        {
            ui->image->setPixmap(QPixmap::fromImage(image));

            ui->horizontalSlider->setMinimum(-reference_image.width());
            ui->horizontalSlider->setMaximum(reference_image.width());

            ui->horizontalSlider_2->setMinimum(-reference_image.height());
            ui->horizontalSlider_2->setMaximum(reference_image.height());
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
                    {0}};

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

void MainWindow::setMatrix(int x, int y)
{
    matrix[0][2] = ui->horizontalSlider->value();
    matrix[1][2] = ui->horizontalSlider_2->value();

    moveVector[0][0] = x;
    moveVector[1][0] = y;

    result[0][0] = 0;
    result[1][0] = 0;
    result[2][0] = 0;


}

void printMatrix()
{
    qDebug() << result[0][0];
    qDebug() << result[1][0];
}

void MainWindow::rasterImageShiftXY(QImage &src, QImage &dst, int v)
{
    dst.fill(Qt::white);
    for (int y = 0; y < src.height(); ++y)
        {
            for (int x = 0; x < src.width(); ++x)
            {
                setMatrix(x,y);
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


