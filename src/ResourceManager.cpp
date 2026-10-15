#include "ResourceManager.h"
#include "Shader.h"

#include <QOpenGLFunctions_3_3_Core>

GLuint ResourceManager::getShader(
    const std::string& vertexPath,
    const std::string& fragmentPath
)
{
    auto key = std::make_pair(vertexPath, fragmentPath);

    auto it = shaderCache.find(key);

    if (it != shaderCache.end())
        return it->second;

    Shader compiler;

    GLuint program = compiler.createShader(
        vertexPath.c_str(),
        fragmentPath.c_str()
    );

    if (program == 0)
        return 0;

    shaderCache.try_emplace(key, program);

    return program;
}



void ResourceManager::releaseGL()
{
    initializeOpenGLFunctions();

    for (const auto& [key, program] : shaderCache)
    {
        glDeleteProgram(program);
    }

    shaderCache.clear();
}