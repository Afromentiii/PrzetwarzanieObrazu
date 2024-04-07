#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setReferenceImage(const QImage &image);
    QImage getReferenceImage();

private:
    Ui::MainWindow *ui;
    QImage reference_image, transformed_image;
public slots:
    void openImage();
    void sliderChanged(int);
    void sliderChanged2(int);
    void rasterImageShiftXY(QImage &src, QImage &dst, int v);
    void setMatrix(int x, int y);
    void multipleMatrix();
};
#endif // MAINWINDOW_H
