#pragma once
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include "Camera.h"

#include <QMouseEvent>


#include <QKeyEvent>
#include <QWheelEvent>
#include <QSet>


class Engine;
class SceneObject;
struct PointLight;
#include <vector>
#include <cstdint>

class Renderer : public QOpenGLWidget,
                 protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit Renderer(Engine *engine);

    Camera camera;

    void tick(float dt);
    void releaseObjectTextures(std::uint64_t id);

signals:
    void selectionChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;



    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;




private:
    void refreshMaterialBuffers(SceneObject* object, const std::vector<PointLight>& nearbyLights);
    void updateDynamicPointLights(GLuint shader, const std::vector<PointLight>& nearbyLights);

    QSet<int> pressedKeys;

    float yaw = -135.0f;
    float pitch = -35.264f;

    float cameraSpeed = 5.0f;
    float mouseSensitivity = 0.2f;

    bool rotatingCamera = false;

    void updateCamera(float dt);
    void updateCameraView();





    GLuint gizmoVAO = 0;
    GLuint gizmoVBO = 0;
    GLuint gizmoShader = 0;

    QPointF lastMousePosition;

    Engine *engine = nullptr;
};