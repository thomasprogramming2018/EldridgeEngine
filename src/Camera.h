#pragma once

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
    QMatrix4x4 uView;
    QMatrix4x4 uProjection;

    QVector3D position = {3.0f, 3.0f, 3.0f};

    Camera();
};