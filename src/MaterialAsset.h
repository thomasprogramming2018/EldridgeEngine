#pragma once
#include <QString>
#include <memory>
class Material;

// Focused, dependency-free .eldmat TOML v1 reader/writer.
// Accepts the keys and value types emitted by save(); not a general TOML parser.
namespace MaterialAsset {
    bool save(const Material& material, const QString& absolutePath, QString* error = nullptr);
    bool load(Material& destination, const QString& absolutePath, QString* error = nullptr);
    // Same .eldmat path returns the same live shared Material in this process.
    std::shared_ptr<Material> loadShared(const QString& absolutePath, QString* error = nullptr);
    void rememberShared(const std::shared_ptr<Material>& material);
}
