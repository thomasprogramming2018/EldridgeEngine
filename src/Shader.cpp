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

std::vector<Buffer> Shader::createPBRMaterial(DirectionalLight dlight, std::vector<PointLight> lights, const char *albedo, const char *specular, const char *normal)
{

    std::vector<Buffer> buffers;
    QVector3D nulll = QVector3D(0.0f, 0.0f, 0.0f);
    QMatrix4x4 nulllmat;
    buffers.push_back(createBuffer(
        "material.diffuse",
        BufferType::SAMPLER2D,
        albedo, 0, 0, nulll, nulllmat
    ));

    buffers.push_back(createBuffer("material.specular", BufferType::SAMPLER2D, specular, 0, 0, nulll, nulllmat));

    buffers.push_back(createBuffer("material.normal", BufferType::SAMPLER2D, normal, 0, 0, nulll, nulllmat));

    buffers.push_back(createBuffer("material.ambient", BufferType::VECTOR3D, nullptr, 0, 0, QVector3D(0.3f, 0.3f, 0.45f), nulllmat));


    for (size_t i = 0; i < lights.size(); i++)
    {
        buffers.push_back(createBuffer(
            "lightCount",
            BufferType::INT,
            nullptr,
            0,
            static_cast<int>(std::min(lights.size(), size_t{32})),
            nulll,
            nulllmat
        ));
        buffers.push_back(
            createBuffer(
                "lights[" + std::to_string(i) + "].ambient",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                lights[i].ambient,
                nulllmat
            )
        );

        buffers.push_back(
            createBuffer(
                "lights[" + std::to_string(i) + "].position",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                lights[i].pos,
                nulllmat
            )
        );

        buffers.push_back(
            createBuffer(
                "lights[" + std::to_string(i) + "].diffuse",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                lights[i].diffuse,
                nulllmat
            )
        );

        buffers.push_back(
            createBuffer(
                "lights[" + std::to_string(i) + "].specular",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                lights[i].specular,
                nulllmat
            )
        );


        

    }

    buffers.push_back(
            createBuffer(
                "dlight.ambient",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                dlight.ambient,
                nulllmat
            )
        );

    buffers.push_back(
            createBuffer(
                "dlight.dir",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                dlight.Dir,
                nulllmat
            )
        );

    buffers.push_back(
            createBuffer(
                "dlight.specular",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                dlight.specular,
                nulllmat
            )
        );

    buffers.push_back(
            createBuffer(
                "dlight.normal",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                dlight.normal,
                nulllmat
            )
        );
    
    buffers.push_back(
            createBuffer(
                "dlight.diffuse",
                BufferType::VECTOR3D,
                nullptr,
                0,
                0,
                dlight.diffuse,
                nulllmat
            )
        );
  

    

    return buffers;
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