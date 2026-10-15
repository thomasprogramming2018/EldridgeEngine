#pragma once

#include "AABB.h"
#include <QVector3D>

#include "Mesh.h"


enum class gState
{
    draggingX,
    draggingY,
    draggingZ,
    None
};


struct GizmosState 
{
    gState dragging_state = gState::None;


};






class Gizmos
{
public:
    AABB aabbx;

    
    AABB aabby;

    AABB aabbz;

    std::unique_ptr<GizmosState> state = std::make_unique<GizmosState>();

    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

    QVector3D position = QVector3D(0.0f, 0.0f, 0.0f);
    
    Gizmos()
    {
        aabbx.min = QVector3D(0.0f, 0.0f, 0.0f);
        aabbx.max = QVector3D(2.0f, 0.3f, 0.3f);

        aabby.min = QVector3D(0.0f, 0.0f, 0.0f);

        aabby.max = QVector3D(0.3f, 2.0f, 0.3f);


        aabbz.min = QVector3D(0.0f, 0.0f, 0.0f);

        aabbz.max = QVector3D(0.3f, 0.3f, 2.0f);


        mesh->vertices = {
            //pos             RGB
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f,
        };




        
    }
    AABB getWorldAABB(const AABB& localBox) const
    {
        return {
            localBox.min + position,
            localBox.max + position
        };
    }
};