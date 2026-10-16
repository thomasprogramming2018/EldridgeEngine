#pragma once
#include <QString>
#include <QVector3D>
#include <cstdint>
#include <vector>
#include "Shader.h"   // Buffer and BufferType, from your existing engine
#include "Light.h"

// CPU-side data only. OpenGL resources are created by Renderer, not by Material.
class Material {
public:
    QString assetPath;  // absolute .eldmat path, empty for unsaved materials
    QString albedoTexture = "src/shaders/container2.png";
    QString specularTexture = "src/shaders/container2_specular.png";
    QString normalTexture = "src/shaders/container2_normal.png";
    QString metallicTexture;    // empty means scalar metallic value is used
    QString roughnessTexture;   // empty means scalar roughness value is used

    QVector3D albedo {1.f, 1.f, 1.f};
    QVector3D specular {1.f, 1.f, 1.f};
    QVector3D normal {0.f, 0.f, 1.f};
    QVector3D ambient {0.3f, 0.3f, 0.45f};
    QVector3D emissive {0.f, 0.f, 0.f};
    float metallic = 0.f;
    float roughness = 0.5f;

    QString vert = "src/shaders/pbr.vert";
    QString frag = "src/shaders/pbr.frag";

    std::uint64_t revision = 1;
    void markDirty() { ++revision; }
    QString resolvePath(const QString& path) const;
    std::vector<Buffer> createPBRMaterial(
        const DirectionalLight& directional,
        const std::vector<PointLight>& nearbyLights) const;
};
