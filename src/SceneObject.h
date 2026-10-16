#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include "Mesh.h"
#include "Shader.h"

#include "ResourceManager.h"







class SceneObject : protected QOpenGLFunctions_3_3_Core
{
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    
    Shader shader;

    GLuint shaderP;





    std::vector<Buffer> buffers;

    std::shared_ptr<Mesh> mesh;

    
    
    SceneObject(
        std::vector<Buffer>& buffers,
        std::shared_ptr<Mesh> sharedMesh
    );



    bool isGpuReady = false;

    void initializeGPU(ResourceManager& resources);


    void updateTransform();

    std::unique_ptr<QMatrix4x4> uModel =
    std::make_unique<QMatrix4x4>();






    AABB getWorldAABB() const
    {
        QVector3D pos(x, y, z);

        return {
            mesh->bounds.min + pos,
            mesh->bounds.max + pos
        };
    }

    SceneObject* parent = nullptr;

    std::vector<SceneObject*> children;


    void setParent(SceneObject* newParent);


    QMatrix4x4 getWorldMatrix() const;

    QMatrix4x4 getLocalMatrix() const;



    



    


};