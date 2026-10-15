#pragma once
#include <QOpenGLFunctions_3_3_Core>



#include <QMatrix4x4>

#include <QVector3D>


#include <QImage>


#include "Light.h"


class SceneObject;



enum class BufferType
{
    MAT4,
    SAMPLER2D,
    VECTOR3D,
    VECTOR2D,
    FLOAT,
    INT
};





struct Buffer
{
    BufferType type;
    

    std::string name;
    

    QMatrix4x4 mat4Val;

    QVector3D vec3Val;

    QImage sampler2dVal;


    GLfloat floatVal;

    GLuint intVal;

    GLuint textureID = 0;
};

class createPBRMaterial : protected QOpenGLFunctions_3_3_Core
{
public:
    

    createPBRMaterial();
    

};



class Shader : protected QOpenGLFunctions_3_3_Core 
{
public:
    GLuint createShader(
        const char* vertex,
        const char* fragment
    );

    std::vector<Buffer> createPBRMaterial(
        DirectionalLight dlight,
        std::vector<PointLight> lights,
        const char* albedo,
        const char* specular,
        const char* normal
    );

    QImage albedo;
    QVector3D albedo_color;

    QImage specular;
    QVector3D specular_intencity;

    QImage normal;
    QVector3D Normal_strength;
    
    Buffer createBuffer(std::string name, BufferType bufferType, const char* sampVal, float fVal, int iVal, QVector3D VVal, QMatrix4x4 mat4Val);
};