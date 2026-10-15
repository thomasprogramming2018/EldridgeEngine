
#include "SceneObject.h"

#include "ResourceManager.h"



#include "Raycast.h"


void SceneObject::updateTransform()
{
    uModel->setToIdentity();
    uModel->translate(x, y, z);
}


SceneObject::SceneObject(
    std::vector<Buffer>& bufferss,
    std::shared_ptr<Mesh> sharedMesh
)
    : buffers(bufferss),
      mesh(std::move(sharedMesh))
{
    

    

}

void SceneObject::initializeGPU(ResourceManager& resources)
{
    if (isGpuReady)
    
        return;
    

    initializeOpenGLFunctions();


    const char *vert = "src/shaders/pbr.vert";

    const char *frag = "src/shaders/pbr.frag";

    shaderP = resources.getShader(vert, frag);

    if (shaderP == 0)
        return;


    if (mesh->VAO == 0)
    {
    // VAO

        glGenVertexArrays(1, &mesh->VAO);

        glBindVertexArray(mesh->VAO);
    }
    // VBO
    if (mesh->VBO == 0)
    {
        glGenBuffers(1, &mesh->VBO);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

        glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float), mesh->vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

        glEnableVertexAttribArray(0);
    }
    
    if (mesh->EBO == 0)
    {
    // EBO

        glGenBuffers(1, &mesh->EBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));

        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    isGpuReady = true;
}