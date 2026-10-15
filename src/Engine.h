#pragma once
#include "SceneObject.h"

#include "Shader.h"

#include "Light.h"

#include "ResourceManager.h"



#include "Gizmos.h"






enum class MaterialType
{
    PBR,
    CUSTOM,
};

class Engine : public Shader
{
private:
public:
    ResourceManager resources;

    Engine();

    void update(float deltaTime);

    void createObject(MaterialType type, std::vector<Buffer> buffers);

    void SceneLight(QVector3D pos, QVector3D diffuse, QVector3D specular, QVector3D ambient, QVector3D normal);

    void initializeGraphics();

    std::vector<std::unique_ptr<SceneObject>> objects;

    std::unique_ptr<Gizmos> gizmos;

    std::unique_ptr<bool> draggingState;



    std::vector<PointLight> lights;

    SceneObject* selectedObject = nullptr;

    DirectionalLight dlight;
};