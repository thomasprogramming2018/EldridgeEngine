
#pragma once

#include "Mesh.h"
#include <memory>

#include <map>
#include <string>
#include <utility>

#include <QOpenGLFunctions_3_3_Core>

class ResourceManager : protected QOpenGLFunctions_3_3_Core
{
private:
    std::map<
        std::pair<std::string, std::string>,
        GLuint
    > shaderCache;
    std::shared_ptr<Mesh> cubeMesh;

public:

    void releaseGL();


    std::shared_ptr<Mesh> getCubeMesh()
    {
        if (!cubeMesh)
        {
            meshObject factory;

            cubeMesh = factory.createCube();
        }



        return cubeMesh;
    }

    GLuint getShader(
        const std::string& vertexPath,
        const std::string& fragmentPath
    );
};
