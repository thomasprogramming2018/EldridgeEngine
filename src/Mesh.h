#pragma once
#include <vector>
#include <QOpenGLFunctions_3_3_Core>
#include <memory>

#include "AABB.h"



struct Mesh
{
    std::vector<float> vertices;

    std::vector<unsigned int> indices;

    GLuint VBO = 0;

    GLuint VAO = 0;

    GLuint EBO = 0;

    AABB bounds;
};
class meshObject
{
public:
    std::unique_ptr<Mesh> createCube()
    {

        AABB bounds{
            QVector3D(-0.5f, -0.5f, -0.5f),
            QVector3D( 0.5f,  0.5f,  0.5f)
        };

        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();


        mesh->bounds = bounds;

        mesh->vertices = {
            // Position            // Normal           // UV

            // Front (+Z)
            -0.5f, -0.5f, 0.5f, 0, 0, 1, 0, 0,
            0.5f, -0.5f, 0.5f, 0, 0, 1, 1, 0,
            0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 1,
            -0.5f, -0.5f, 0.5f, 0, 0, 1, 0, 0,
            0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 1,
            -0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 1,

            // Back (-Z)
            0.5f, -0.5f, -0.5f, 0, 0, -1, 0, 0,
            -0.5f, -0.5f, -0.5f, 0, 0, -1, 1, 0,
            -0.5f, 0.5f, -0.5f, 0, 0, -1, 1, 1,
            0.5f, -0.5f, -0.5f, 0, 0, -1, 0, 0,
            -0.5f, 0.5f, -0.5f, 0, 0, -1, 1, 1,
            0.5f, 0.5f, -0.5f, 0, 0, -1, 0, 1,

            // Left (-X)
            -0.5f, -0.5f, -0.5f, -1, 0, 0, 0, 0,
            -0.5f, -0.5f, 0.5f, -1, 0, 0, 1, 0,
            -0.5f, 0.5f, 0.5f, -1, 0, 0, 1, 1,
            -0.5f, -0.5f, -0.5f, -1, 0, 0, 0, 0,
            -0.5f, 0.5f, 0.5f, -1, 0, 0, 1, 1,
            -0.5f, 0.5f, -0.5f, -1, 0, 0, 0, 1,

            // Right (+X)
            0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 0,
            0.5f, -0.5f, -0.5f, 1, 0, 0, 1, 0,
            0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 1,
            0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 0,
            0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 1,
            0.5f, 0.5f, 0.5f, 1, 0, 0, 0, 1,

            // Top (+Y)
            -0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 0,
            0.5f, 0.5f, 0.5f, 0, 1, 0, 1, 0,
            0.5f, 0.5f, -0.5f, 0, 1, 0, 1, 1,
            -0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 0,
            0.5f, 0.5f, -0.5f, 0, 1, 0, 1, 1,
            -0.5f, 0.5f, -0.5f, 0, 1, 0, 0, 1,

            // Bottom (-Y)
            -0.5f, -0.5f, -0.5f, 0, -1, 0, 0, 0,
            0.5f, -0.5f, -0.5f, 0, -1, 0, 1, 0,
            0.5f, -0.5f, 0.5f, 0, -1, 0, 1, 1,
            -0.5f, -0.5f, -0.5f, 0, -1, 0, 0, 0,
            0.5f, -0.5f, 0.5f, 0, -1, 0, 1, 1,
            -0.5f, -0.5f, 0.5f, 0, -1, 0, 0, 1};

        mesh->indices = {
            0, 1, 2, 3, 4, 5,       // Front
            6, 7, 8, 9, 10, 11,     // Back
            12, 13, 14, 15, 16, 17, // Left
            18, 19, 20, 21, 22, 23, // Right
            24, 25, 26, 27, 28, 29, // Top
            30, 31, 32, 33, 34, 35  // Bottom
        };

        return mesh;
    }
};