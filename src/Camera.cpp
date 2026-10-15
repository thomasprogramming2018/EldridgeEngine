#include "Camera.h"

Camera::Camera()
{
    uView.lookAt(
        position,
        QVector3D(0, 0, 0),
        QVector3D(0, 1, 0)
    );

    uProjection.perspective(
        60.0f,   // FOV
        16.0f / 9.0f, // Aspect ratio
        0.1f,    // Near plane
        100.0f   // Far plane
    );
}