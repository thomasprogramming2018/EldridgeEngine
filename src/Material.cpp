#include "Material.h"
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <algorithm>
#include <string>
#include <utility>

QString Material::resolvePath(const QString& path) const {
    if (path.trimmed().isEmpty()) return {};
    if (QFileInfo(path).isAbsolute()) return QDir::cleanPath(path);
    const QDir base = assetPath.isEmpty() ? QDir::current() : QFileInfo(assetPath).absoluteDir();
    return QDir::cleanPath(base.absoluteFilePath(path));
}

std::vector<Buffer> Material::createPBRMaterial(
    const DirectionalLight& directional,
    const std::vector<PointLight>& nearbyLights) const
{
    std::vector<Buffer> out;
    auto scalar = [&](const std::string& name, float value) {
        Buffer b{}; b.name = name; b.type = BufferType::FLOAT;
        b.floatVal = value; out.push_back(std::move(b));
    };
    auto integer = [&](const std::string& name, int value) {
        Buffer b{}; b.name = name; b.type = BufferType::INT;
        b.intVal = static_cast<GLuint>(value); out.push_back(std::move(b));
    };
    auto vector = [&](const std::string& name, const QVector3D& value) {
        Buffer b{}; b.name = name; b.type = BufferType::VECTOR3D;
        b.vec3Val = value; out.push_back(std::move(b));
    };
    auto texture = [&](const std::string& name, const QString& path) {
        if (path.trimmed().isEmpty()) return false;
        QImage image(resolvePath(path));
        if (image.isNull()) return false;
        Buffer b{}; b.name = name; b.type = BufferType::SAMPLER2D;
        b.sampler2dVal = std::move(image); b.textureID = 0;
        out.push_back(std::move(b));
        return true;
    };
    const bool hasAlbedo = texture("material.diffuse", albedoTexture);
    const bool hasSpecular = texture("material.specular", specularTexture);
    const bool hasNormal = texture("material.normal", normalTexture);
    const bool hasMetallic = texture("material.metallic_tex", metallicTexture);
    const bool hasRoughness = texture("material.roughness_tex", roughnessTexture);
    // GLSL will ignore uniforms that its shader does not declare.
    integer("material.hasDiffuseMap", hasAlbedo);
    integer("material.hasSpecularMap", hasSpecular);
    integer("material.hasNormalMap", hasNormal);
    integer("material.hasMetallicMap", hasMetallic);
    integer("material.hasRoughnessMap", hasRoughness);
    vector("material.albedo", albedo);
    vector("material.specular_color", specular);
    vector("material.normal_vector", normal);
    vector("material.ambient", ambient);
    vector("material.emissive", emissive);
    scalar("material.metallic", metallic);
    scalar("material.roughness", roughness);
    scalar("material.normalStrength", normalStrength);

    const auto count = std::min(nearbyLights.size(), std::size_t{32});
    integer("lightCount", static_cast<int>(count));
    for (std::size_t i = 0; i < count; ++i) {
        const std::string prefix = "lights[" + std::to_string(i) + "].";
        vector(prefix + "ambient", nearbyLights[i].ambient);
        vector(prefix + "position", nearbyLights[i].pos);
        vector(prefix + "diffuse", nearbyLights[i].diffuse);
        vector(prefix + "specular", nearbyLights[i].specular);
    }
    vector("dlight.ambient", directional.ambient);
    vector("dlight.dir", directional.Dir);
    vector("dlight.diffuse", directional.diffuse);
    vector("dlight.normal", directional.normal);
    vector("dlight.specular", directional.specular);
    return out;
}
