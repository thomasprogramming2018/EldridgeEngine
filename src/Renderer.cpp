#include "Renderer.h"
#include "Engine.h"

#include "Raycast.h"

#include <vector>

#include <QtMath>
#include <algorithm>

void Renderer::updateCameraView()
{
    float y = qDegreesToRadians(yaw);
    float p = qDegreesToRadians(pitch);

    QVector3D forward(
        cos(p) * cos(y),
        sin(p),
        cos(p) * sin(y)
    );

    camera.uView.setToIdentity();
    camera.uView.lookAt(
        camera.position,
        camera.position + forward,
        QVector3D(0, 1, 0)
    );
}

void Renderer::updateCamera(float dt)
{
    float y = qDegreesToRadians(yaw);
    float p = qDegreesToRadians(pitch);

    QVector3D forward(
        cos(p) * cos(y),
        sin(p),
        cos(p) * sin(y)
    );

    QVector3D right = QVector3D::crossProduct(
        forward,
        QVector3D(0, 1, 0)
    ).normalized();

    QVector3D up(0, 1, 0);

    float speed = cameraSpeed;

    if (pressedKeys.contains(Qt::Key_Shift))
        speed *= 3.0f;

    QVector3D movement;

    if (pressedKeys.contains(Qt::Key_W)) movement += forward;
    if (pressedKeys.contains(Qt::Key_S)) movement -= forward;
    if (pressedKeys.contains(Qt::Key_D)) movement += right;
    if (pressedKeys.contains(Qt::Key_A)) movement -= right;
    if (pressedKeys.contains(Qt::Key_E)) movement += up;
    if (pressedKeys.contains(Qt::Key_Q)) movement -= up;

    if (!movement.isNull())
        camera.position += movement.normalized() * speed * dt;

    updateCameraView();
}


void Renderer::keyPressEvent(QKeyEvent* event)
{
    if (!event->isAutoRepeat())
        pressedKeys.insert(event->key());
}

void Renderer::keyReleaseEvent(QKeyEvent* event)
{

    if (!event->isAutoRepeat())
        pressedKeys.remove(event->key());
}

void Renderer::wheelEvent(QWheelEvent* event)
{
    float amount = event->angleDelta().y() / 120.0f;
    camera.position +=
        camera.uView.inverted().mapVector(QVector3D(0, 0, -1))
            * amount * 0.5f;

    updateCameraView();
}





Renderer::Renderer(Engine* engine)
    : engine(engine)
{
}


void Renderer::tick(float dt)
{
    updateCamera(dt);
}

void Renderer::mouseMoveEvent(QMouseEvent* event)
{
    QPointF delta = event->position() - lastMousePosition;
    lastMousePosition = event->position();

    if (rotatingCamera)
    {
        yaw += delta.x() * mouseSensitivity;
        pitch -= delta.y() * mouseSensitivity;

        pitch = std::clamp(pitch, -89.0f, 89.0f);

        updateCameraView();
        update();
        return;
    }




    if (!engine->selectedObject)
        return;

    if (engine->selectedObject &&
    engine->gizmos->state->dragging_state == gState::draggingX)
    {
        engine->selectedObject->x += delta.x() * 0.01f;
        engine->selectedObject->updateTransform();

        engine->gizmos->position.setX(engine->selectedObject->getWorldMatrix().column(3).x());

        qDebug() << "Object X:" << engine->selectedObject->x;
        update();
    }

    if (engine->selectedObject &&
    engine->gizmos->state->dragging_state == gState::draggingY)
    {
        engine->selectedObject->y += -delta.y() * 0.01f;
        engine->selectedObject->updateTransform();

        engine->gizmos->position.setY(engine->selectedObject->getWorldMatrix().column(3).y());

        qDebug() << "Object X:" << engine->selectedObject->y;
        update();
    }

    if (engine->selectedObject &&
    engine->gizmos->state->dragging_state == gState::draggingZ)
    {
        engine->selectedObject->z += delta.y() * 0.01f;
        engine->selectedObject->updateTransform();

        engine->gizmos->position.setZ(engine->selectedObject->getWorldMatrix().column(3).z());

        qDebug() << "Object X:" << engine->selectedObject->z;
        update();
    }


}


void Renderer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        rotatingCamera = false;
        return;
    }
    Q_UNUSED(event);
    engine->gizmos->state->dragging_state = gState::None;
}


void Renderer::mousePressEvent(QMouseEvent* event)
{



    setFocus();

    if (event->button() == Qt::RightButton)
    {
        rotatingCamera = true;
        lastMousePosition = event->position();
        return;
    }


    lastMousePosition = event->position();


    // 1) Mouse click in Qt pixel coordinates
    QPointF mouse = event->position();

    // 2) Convert pixels into normalized screen coordinates
    float ndcX = (2.0f * mouse.x() / width()) - 1.0f;
    float ndcY = 1.0f - (2.0f * mouse.y() / height());

    // 3) Make a point on the far side of the screen space "box"
    QVector4D clipPoint(ndcX, ndcY, 1.0f, 1.0f);

    // 4) Reverse the camera+projection transform
    QMatrix4x4 inverseVP =
        (camera.uProjection * camera.uView).inverted();

    QVector4D worldPoint4 = inverseVP * clipPoint;

    // 5) Convert from 4D homogeneous coords into normal 3D coords
    QVector3D worldPoint = worldPoint4.toVector3DAffine();

    // 6) Build the ray
    QVector3D rayOrigin = camera.position;
    QVector3D rayDirection = (worldPoint - rayOrigin).normalized();


    QVector3D shoot = camera.position + rayDirection * 3000;



    auto& gizmo = *engine->gizmos;

    // Check gizmo BEFORE clearing the selected object
    if (engine->selectedObject)
    {
        if (rayAABB(rayOrigin, rayDirection,
                    gizmo.getWorldAABB(gizmo.aabbx)))
        {
            gizmo.state->dragging_state = gState::draggingX;
            return;
        }

        if (rayAABB(rayOrigin, rayDirection,
                    gizmo.getWorldAABB(gizmo.aabby)))
        {
            gizmo.state->dragging_state = gState::draggingY;
            return;
        }

        if (rayAABB(rayOrigin, rayDirection,
                    gizmo.getWorldAABB(gizmo.aabbz)))
        {
            gizmo.state->dragging_state = gState::draggingZ;
            return;
        }
    }

    // Otherwise select an object
    engine->selectedObject = nullptr;
    gizmo.state->dragging_state = gState::None;

    for (auto& obj : engine->objects)
    {
        if (rayAABB(rayOrigin, rayDirection,
                    obj->getWorldAABB()))
        {
            engine->selectedObject = obj.get();

            gizmo.position = QVector3D(
                obj->x,
                obj->y,
                obj->z
            );

            break;
        }
    }


    

    qDebug() << "shoot pos: " << shoot;

    qDebug() << "-------------------------";
    qDebug() << "Mouse pixels:" << mouse;
    qDebug() << "NDC:" << ndcX << ndcY;
    qDebug() << "Clip point:" << clipPoint;
    qDebug() << "World point on far plane:" << worldPoint;
    qDebug() << "Ray origin:" << rayOrigin;
    qDebug() << "Ray direction:" << rayDirection;
}




void Renderer::resizeGL(int w, int h)
{
    if (h == 0) return;

    glViewport(0, 0, w, h);

    camera.uProjection.setToIdentity();
    camera.uProjection.perspective(
        60.0f,
        float(w) / float(h),
        0.1f,
        100.0f
    );
}

void Renderer::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); 


    //Gizmos start

    auto& mesh = engine->gizmos->mesh;

    gizmoShader = engine->resources.getShader(
        "src/shaders/vector.vert",
        "src/shaders/vector.frag"
    );
    
    glGenVertexArrays(1, &gizmoVAO);
    glGenBuffers(1, &gizmoVBO);
    
    glBindVertexArray(gizmoVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
    
    glBufferData(
        GL_ARRAY_BUFFER,
        mesh->vertices.size() * sizeof(float),
        mesh->vertices.data(),
        GL_STATIC_DRAW
    );
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    
    // RGB
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    //gizmos end


    

    engine->initializeGraphics();


    

    for (auto& obj : engine->objects)
    {

        
        for (Buffer& buffer : obj->buffers)
        {
            if (buffer.type != BufferType::SAMPLER2D)
                continue;

            QImage image = buffer.sampler2dVal
                .convertToFormat(QImage::Format_RGBA8888)
                .mirrored(false, true);

            if (image.isNull())
            {
                qWarning() << "Texture failed:"
                           << buffer.name.c_str();
                continue;
            }

            glGenTextures(1, &buffer.textureID);
            glBindTexture(GL_TEXTURE_2D, buffer.textureID);

            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA8,
                image.width(), image.height(),
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                image.constBits()
            );

            glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_S, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_T, GL_REPEAT);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::paintGL()
{
    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto& obj : engine->objects)
    {

        if (!obj->isGpuReady)
        {
            obj->initializeGPU(engine->resources);
        }

        glUseProgram(obj->shaderP);

        GLint location2 = glGetUniformLocation(
                    obj->shaderP,
                    "uProjection"
                );
        auto setMat4 = [&](const char* name, const QMatrix4x4& mat)
        {
            GLint loc = glGetUniformLocation(obj->shaderP, name);       

            if (loc != -1)
                glUniformMatrix4fv(loc, 1, GL_FALSE, mat.constData());
        };   
        
        GLint loc = glGetUniformLocation(
                    obj->shaderP,
                    "camera.Pos"
                );

        if (loc != -1)
        {
            glUniform3f(
                loc,
                camera.position.x(),
                camera.position.y(),
                camera.position.z()
            );

        }


        setMat4("uModel", obj->getWorldMatrix());
        setMat4("uView", camera.uView);
        setMat4("uProjection", camera.uProjection);
        int textureUnit = 0;

        for (const Buffer& buffer : obj->buffers)
        {
            GLint location = glGetUniformLocation(
                obj->shaderP,
                buffer.name.c_str()
            );

            if (location == -1)
                continue;

            switch (buffer.type)
            {
                case BufferType::VECTOR3D:
                    glUniform3f(
                        location,
                        buffer.vec3Val.x(),
                        buffer.vec3Val.y(),
                        buffer.vec3Val.z()
                    );
                    break;

                case BufferType::FLOAT:
                    glUniform1f(location, buffer.floatVal);
                    break;

                case BufferType::INT:
                    glUniform1i(location, buffer.intVal);
                    break;

                case BufferType::MAT4:
                    glUniformMatrix4fv(
                        location, 1, GL_FALSE,
                        buffer.mat4Val.constData()
                    );
                    break;

                case BufferType::SAMPLER2D:
                    if (buffer.textureID != 0)
                    {
                        glActiveTexture(GL_TEXTURE0 + textureUnit);
                        glBindTexture(GL_TEXTURE_2D, buffer.textureID);
                        glUniform1i(location, textureUnit);
                        textureUnit++;
                    }
                    break;
            }
        }

        glBindVertexArray(obj->mesh->VAO);

        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(obj->mesh->indices.size()),
            GL_UNSIGNED_INT,
            nullptr
        );

        glBindVertexArray(0);
    }

    if (engine->selectedObject &&
    gizmoShader != 0 &&
    gizmoVAO != 0)
    {

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);



        
        glUseProgram(gizmoShader);
        
        
        QMatrix4x4 model;
        model.setToIdentity();
        model.translate(engine->gizmos->position);
        
        auto setMatrix = [&](const char* name, const QMatrix4x4& mat)
        {
            GLint loc = glGetUniformLocation(gizmoShader, name);
            if (loc != -1)
            glUniformMatrix4fv(loc, 1, GL_FALSE, mat.constData());
        };
        
        setMatrix("uModel", model);
        setMatrix("uView", camera.uView);
        setMatrix("uProjection", camera.uProjection);
        
        glBindVertexArray(gizmoVAO);
        
        glDrawArrays(GL_LINES, 0, 6);
        
        glBindVertexArray(0);
        // Restore OpenGL state
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }
}