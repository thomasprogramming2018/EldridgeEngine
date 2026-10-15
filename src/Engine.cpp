#include "Engine.h"
#include <QDebug>

#include "SceneObject.h"
#include "Shader.h"




void Engine::initializeGraphics()
{

    std::vector<Buffer> empty;
    createObject(MaterialType::PBR, empty);

}





Engine::Engine()
{



    SceneLight(
        QVector3D(0.0f, 1.5f, 0.0f),
        QVector3D(0.7f, 0.8f, 0.3f),
        QVector3D(1.0f, 1.0f, 1.0f),
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(1.0f, 1.0f, 1.0f)
    );

    DirectionalLight dlight1;

    dlight1.ambient = QVector3D(0.3f, 0.3f, 0.45f);

    dlight1.diffuse = QVector3D(0.7f, 0.8f, 0.3f);

    dlight1.Dir = QVector3D(0.5f, 1.0f, 0.5f);

    dlight1.normal = QVector3D(1.0f, 1.0f, 1.0f);

    dlight1.specular = QVector3D(1.0f, 1.0f, 1.0f);

    dlight = dlight1;


    gizmos = std::make_unique<Gizmos>();


}

void Engine::SceneLight(QVector3D pos, QVector3D diffuse, QVector3D specular, QVector3D ambient, QVector3D normal)
{
    PointLight point;

    point.pos = pos;

    point.diffuse = diffuse;

    point.specular = specular;

    point.ambient = ambient;

    point.normal = normal;

    lights.push_back(point);
}

void Engine::createObject(
    MaterialType type,
    std::vector<Buffer> buffers
)
{
    if (type == MaterialType::PBR && buffers.empty())
    {
        buffers = createPBRMaterial(
            dlight,
            lights,
            "src/shaders/container2.png",
            "src/shaders/container2_specular.png",
            "src/shaders/container2_normal.png"
        );
    }

    auto mesh = resources.getCubeMesh();

    objects.push_back(
        std::make_unique<SceneObject>(buffers, mesh)
    );


}
    //if (type == MaterialType::PBR)
    //{
        Shader shader;

    
        //buffers = createPBRMaterial(dlight, lights, "src/shaders/container2.png", "src/shaders/container2_specular.png", "src/shaders/container2_normal.png");

    
        //SceneObject obj1(buffers);

        

        //objects.push_back(std::make_unique<SceneObject>(buffers));



    //}
//}




void Engine::update(float deltaTime)
{

}