#include "glwidget.h"
#include <QFile>
#include <QTextStream>
float lastY = 0.0f;
float lastX = 0.0f;
float pitch = 0.0f;
float yaw = 0.0f;
bool m_firstMouse = true;
float m_yaw = -90.0f;
float m_pitch = 0.0f;
float m_lastX = 400.0f;
float m_lastY = 300.0f;
class Frame
{
public:
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 pos;
    glm::vec3 scale;

    Frame() : up(glm::vec3(0,1,0)), forward(glm::vec3(0,0,-1)), pos(glm::vec3(0,0,0)) {}

    glm::vec3 s()
    {
        return glm::cross(up, forward);
    }

    glm::mat4 matrix()
    {
        glm::mat4 M(1.0f);
        glm::vec3 f = glm::normalize(forward);
        glm::vec3 xx = glm::normalize(s());
        glm::vec3 upp = glm::normalize(glm::cross(forward, xx));

        M[0] = glm::vec4(xx, 0);
        M[1] = glm::vec4(upp, 0);
        M[2] = glm::vec4(f, 0);
        M[3] = glm::vec4(pos, 1);

        return M;
    }
};

class Camera : public Frame
{
public:
    Camera() {}
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    float cameraAngle = 179.0f;

};


Camera camera = Camera();


void rotateLocalX(float angle)
{
    ;
}

GLWidget::GLWidget() { }

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    const float cameraSpeed = 0.05f;
    if (event->key() == Qt::Key_W)
        camera.cameraPos += cameraSpeed * camera.cameraFront;
    if (event->key() == Qt::Key_S)
    {
        camera.cameraPos -= cameraSpeed * camera.cameraFront;
    }
    if (event->key() == Qt::Key_A)
        camera.cameraPos -= glm::cross(camera.cameraFront, camera.cameraUp) * cameraSpeed;
    if (event->key() == Qt::Key_D)
        camera.cameraPos += glm::cross(camera.cameraFront, camera.cameraUp) * cameraSpeed;

    update();
}

void GLWidget::initializeGL()
{
    lastX = float(width() / 2.0f);
    lastY = float(height() / 2.0f);
    initializeOpenGLFunctions();
    programs["basic_shader"] = new GLSLProgram();
    programs["basic_shader"]->compileShaderFromFile("./shaders/shader.vert", GL_VERTEX_SHADER);
    programs["basic_shader"]->compileShaderFromFile("./shaders/shader.frag", GL_FRAGMENT_SHADER);
    programs["basic_shader"]->link();
    //....

    // jakas geometria
    geometries["axes"] = new Geometry();
    geometries["axes"]->primitiveMode = GL_LINES;
    glm::vec3 verts[] = { {0,0,0}, {0.5,0,0}, {0,0,0}, {0,0.5,0}, {0,0,0}, {0,0,0.5} };
    glm::vec3 colors[] = { {1,0,0}, {1,0,0},  {0,1,0}, {0,1,0},   {0,0,1}, {0,0,1} };
    geometries["axes"]->setVertices(0, verts, 6);
    geometries["axes"]->setAttribute(1, colors, 6);

    /*
    v0    v1
    *-----*
    |   / |
    | /   |
    *-----*
    v3    v2
  */

    geometries["plane"] = new Geometry();
    float plane_size = 0.1;
    geometries["plane"]->primitiveMode = GL_TRIANGLES;
    glm::vec3 plane_verts[] = {{-plane_size,  plane_size,0},
                               { plane_size,  plane_size,0},
                               { plane_size, -plane_size,0},
                               {-plane_size, -plane_size,0}};
    glm::vec3 plane_colors[] = {{1,0,1}, {0,1,1}, {1,1,0}, {1,0,0} };
    uint plane_idxs[] = {0,3,1,    1,3,2};
    geometries["plane"]->setVertices(0, plane_verts, 4);
    geometries["plane"]->setAttribute(1, plane_colors, 4);
    geometries["plane"]->setIndices(plane_idxs, 6);


    timer.setInterval(1);
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start();

    PRINT_GL_ERRORS("Widget::initializeGL(): ");
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    // Współrzędne środka sceny
    int sceneCenterX = width() / 2;
    int sceneCenterY = height() / 2;

    if (m_firstMouse) {
        m_lastX = sceneCenterX; // Ustaw ostatnią pozycję myszy na środek sceny
        m_lastY = sceneCenterY;
        m_firstMouse = false;
    }

    float xOffset = e->pos().x() - m_lastX; // Oblicz przesunięcie myszy względem poprzedniej pozycji
    float yOffset = m_lastY - e->pos().y();
    m_lastX = e->pos().x(); // Zapisz bieżącą pozycję myszy
    m_lastY = e->pos().y();

    const float sensitivity = 0.1f;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    if (m_yaw > 360.0f)
        m_yaw -= 360.0f;
    if (m_yaw < 0.0f)
        m_yaw += 360.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    camera.cameraFront = glm::normalize(front);

    update();
}




void GLWidget::paintGL()
{
    glClearColor(0.5, 0.5, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // położenie kamery,
   // punkt środkowy ekranu,
   // oś "do góry" - tutaj do góry jest skierowana oś Oy .

    glm::mat4 view_mat = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
    glm::mat4 model_mat = glm::mat4(1.0);
    glm::mat4 MV = glm::mat4(1.0);

    float fov = 55.0f;

    glm::mat4 projection = glm::mat4(1.0);
    projection = glm::perspective(glm::radians(fov), float(width()) / float(height()), 0.1f, 100.0f);

    programs["basic_shader"]->use();
    programs["basic_shader"]->setUniform("ProjectionMat", projection);
    programs["basic_shader"]->setUniform("MVMat", MV);
    programs["basic_shader"]->setUniform("View", view_mat);
    geometries["axes"]->render();
    geometries["plane"]->render();

    Frame movement_sun = Frame();
    model_mat = movement_sun.matrix();
    movement_sun.forward = glm::vec3(0,0,1);
    model_mat = glm::rotate(model_mat, angle / 100.0f, movement_sun.forward);
    movement_sun.pos = glm::vec3(0.5, 0.0, 0.0);
    model_mat = glm::translate(model_mat, movement_sun.pos);
    model_mat = glm::rotate(model_mat, -angle/100.f,  movement_sun.forward);
    movement_sun.forward  = glm::vec3(1,0,0);
    model_mat = glm::rotate(model_mat, glm::radians(33.0f), movement_sun.forward);
    movement_sun.forward = glm::vec3(0,0,1);
    model_mat = glm::rotate(model_mat, angle/20.f, movement_sun.forward);
    movement_sun.scale = glm::vec3(0.5, 0.5, 0.5);
    model_mat =  glm::scale(model_mat, movement_sun.scale);

    programs["basic_shader"]->use();
    programs["basic_shader"]->setUniform("MVMat", model_mat);
    geometries["axes"]->render();
    geometries["plane"]->render();

    angle++;
}
