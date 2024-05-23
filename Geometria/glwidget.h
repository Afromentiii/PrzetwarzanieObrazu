#ifndef GLWIDGET_H
#define GLWIDGET_H

#if QT_VERSION_MAJOR >= 6
    #include <QtOpenGLWidgets/QOpenGLWidget>
#else
    #include <QOpenGLWidget>
#endif

#include<QMouseEvent>
#include "glslprogram.h"
#include "geometry.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QMap>
#include <QDebug>
#include <QTimer>

#include "mathgl.h"


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    QMap<QString, GLSLProgram*> programs;
    QMap<QString, Geometry*> geometries;

    mat4 MVMat;
    mat4 ProjMat;
    float angle;
    QTimer timer;
    int pos_x, pos_y;

    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void keyPressEvent(QKeyEvent *event);
public:
    GLWidget();
};


#endif // GLWIDGET_H
