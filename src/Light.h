#pragma once


#include <QVector3D>






struct PointLight
{
    QVector3D pos;


    QVector3D diffuse;

    QVector3D specular;

    QVector3D ambient;
     
    QVector3D normal;
    
};




struct DirectionalLight
{
    QVector3D Dir;


    QVector3D diffuse;

    QVector3D specular;

    QVector3D ambient;
     
    QVector3D normal;
    
};