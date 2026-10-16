#include "Shader.h"

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

#include "SceneObject.h"




std::string readShader(const std::string &path)
{
    std::ifstream file(path);

    if (!file.is_open())
        throw std::runtime_error("Cannot open shader: " + path);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}




GLuint Shader::createShader(
    const char* vertex,
    const char* fragment
)
{
    initializeOpenGLFunctions();

    std::string vertexCode = readShader(vertex);
    std::string fragmentCode = readShader(fragment);

    const char *vertexShaderSource = vertexCode.c_str();
    const char *fragmentShaderSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    glCompileShader(vertexShader);

    glCompileShader(fragmentShader);

    GLint success;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);

    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        char log[512];

        glGetProgramInfoLog(program, 512, nullptr, log);

        std::cerr << log << std::endl;
    }
    {
        qDebug() << "Shaders loaded Succesfuly!";
    }

    glDeleteShader(vertexShader);

    glDeleteShader(fragmentShader);

    return program;
}

Buffer Shader::createBuffer(std::string name, BufferType bufferType, const char *sampVal, float fVal, int iVal, QVector3D VVal, QMatrix4x4 mat4Val)
{
    switch (bufferType)
    {
    case BufferType::SAMPLER2D:
    {
        Buffer textureBuffer;
        textureBuffer.name = name;
        textureBuffer.type = BufferType::SAMPLER2D;

        textureBuffer.sampler2dVal = QImage(sampVal);

        return textureBuffer;
    }
    case BufferType::VECTOR3D:
    {
        Buffer vectorVal;

        vectorVal.name = name;

        vectorVal.type = BufferType::VECTOR3D;

        vectorVal.vec3Val = VVal;

        return vectorVal;
    }

    case BufferType::MAT4:
    {
        Buffer mat4Value;

        mat4Value.name = name;

        mat4Value.type = BufferType::MAT4;

        mat4Value.mat4Val = mat4Val;
        return mat4Value;
    }

    case BufferType::FLOAT:
    {
        Buffer floatValue;

        floatValue.name = name;

        floatValue.type = BufferType::FLOAT;

        floatValue.floatVal = fVal;
        return floatValue;
    }

    case BufferType::INT:
    {
        Buffer intValue;

        intValue.name = name;

        intValue.type = BufferType::INT;

        intValue.intVal = iVal;
        return intValue;
    }
    }
    Buffer nullbuff;
    return nullbuff;
}