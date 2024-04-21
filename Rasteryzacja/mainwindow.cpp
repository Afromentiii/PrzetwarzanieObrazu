#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qevent.h"
#include <QImage>
#include <QLabel>
#include <QSinglePointEvent>

QImage Canva;

class DerivedWidget : public QLabel
{
public:

    int indexLine = 0;
    QPoint pointArrayLine[2];

    int indexCircle = 0;
    QPoint pointArrayCircle[2];

    DerivedWidget()
    {
        Canva = QImage(1895, 890, QImage::Format_RGB32);

        this->setFixedSize(1895, 890);
        this->setMouseTracking(false);
        this->setPixmap(QPixmap::fromImage(Canva));
        this->setCursor(Qt::PointingHandCursor);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            //pierwsze klikniecie wybiera pierwszy punkt, drugie klikniecie wybiera drugi punkt
            QPoint p = QPoint(event->pos().x(), event->pos().y());
            qDebug() << "x: " <<  p.x() << "y: " << p.y();
            pointArrayLine[indexLine++] = p;

            if (indexLine == 2)
            {
                qDebug() << pointArrayLine[0] << pointArrayLine[1];
                indexLine = 0;
                drawLine(pointArrayLine[0].x(), pointArrayLine[0].y(), pointArrayLine[1].x(), pointArrayLine[1].y());
            }
        }

        else if(event->button() == Qt::RightButton)
        {
            //pierwsze klikniecie punkt startowy, drugie klikniecie to punkt nr 2 z ktorego liczony jest promien
            QPoint p = QPoint(event->pos().x(), event->pos().y());
            qDebug() << "x: " <<  p.x() << "y: " << p.y();
            pointArrayCircle[indexCircle++] = p;

            if (indexCircle == 2)
            {
                indexCircle = 0;
                QLineF line(pointArrayCircle[0], pointArrayCircle[1]);
                qreal length = line.length();

                drawCircle(pointArrayCircle[0].x(), pointArrayCircle[0].y(), int(length));
            }
        }
        this->setPixmap(QPixmap::fromImage(Canva));
    }

    void drawLine(int x1, int y1, int x2, int y2)
    {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

        float incrementX = static_cast<float>(dx) / static_cast<float>(steps);
        float incrementY = static_cast<float>(dy) / static_cast<float>(steps);

        float x = x1;
        float y = y1;

        for (int i = 0; i <= steps; ++i)
        {
            Canva.setPixelColor(static_cast<int>(x), static_cast<int>(y), Qt::green);
            x += incrementX;
            y += incrementY;
        }
    }

    void drawPixel(int centerX, int centerY, int x, int y)
    {
        int canvasWidth = Canva.width();
        int canvasHeight = Canva.height();

        if (centerX + x >= 0 && centerX + x < canvasWidth &&
            centerY + y >= 0 && centerY + y < canvasHeight)
        {
            Canva.setPixelColor(centerX + x, centerY + y, Qt::green);
        }

        if (centerX - x >= 0 && centerX - x < canvasWidth &&
            centerY + y >= 0 && centerY + y < canvasHeight)
        {
            Canva.setPixelColor(centerX - x, centerY + y, Qt::green);
        }

        if (centerX + x >= 0 && centerX + x < canvasWidth &&
            centerY - y >= 0 && centerY - y < canvasHeight)
        {
            Canva.setPixelColor(centerX + x, centerY - y, Qt::green);
        }

        if (centerX - x >= 0 && centerX - x < canvasWidth &&
            centerY - y >= 0 && centerY - y < canvasHeight)
        {
            Canva.setPixelColor(centerX - x, centerY - y, Qt::green);
        }

        if (centerX + y >= 0 && centerX + y < canvasWidth &&
            centerY + x >= 0 && centerY + x < canvasHeight)
        {
            Canva.setPixelColor(centerX + y, centerY + x, Qt::green);
        }

        if (centerX - y >= 0 && centerX - y < canvasWidth &&
            centerY + x >= 0 && centerY + x < canvasHeight)
        {
            Canva.setPixelColor(centerX - y, centerY + x, Qt::green);
        }

        if (centerX + y >= 0 && centerX + y < canvasWidth &&
            centerY - x >= 0 && centerY - x < canvasHeight)
        {
            Canva.setPixelColor(centerX + y, centerY - x, Qt::green);
        }

        if (centerX - y >= 0 && centerX - y < canvasWidth &&
            centerY - x >= 0 && centerY - x < canvasHeight)
        {
            Canva.setPixelColor(centerX - y, centerY - x, Qt::green);
        }
    }

    void drawCircle(int centerX, int centerY, int radius)
    {
        int x = radius;
        int y = 0;
        int decision = 1 - radius;

        drawPixel(centerX, centerY, x, y);

        while (y <= x)
        {
            y++;
            if (decision <= 0)
            {
                decision += 2 * y + 1;
            }

            else
            {
                x--;
                decision += 2 * (y - x) + 1;
            }
            drawPixel(centerX, centerY, x, y);
        }
    }
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    DerivedWidget *label = new DerivedWidget();
    ui->verticalLayout->addWidget(label);
}



MainWindow::~MainWindow()
{
    delete ui;
}
