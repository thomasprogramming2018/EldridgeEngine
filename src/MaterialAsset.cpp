#include "MaterialAsset.h"
#include "Material.h"
#include <QDir>
#include <QHash>
#include <memory>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QSaveFile>
#include <QSet>
#include <QTextStream>
#include <QtGlobal>
#include <cmath>

namespace {
QHash<QString, std::weak_ptr<Material>>& assetCache() {
    static QHash<QString, std::weak_ptr<Material>> cache;
    return cache;
}
QString quote(const QString& text) {
    QJsonArray arr; arr.append(text);
    QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
    return json.mid(1, json.size() - 2);
}
QString number(double d) { return QString::number(d, 'g', 9); }
QString vec(const QVector3D& v) {
    return "[" + number(v.x()) + ", " + number(v.y()) + ", " + number(v.z()) + "]";
}
void fail(QString* error, const QString& message) { if (error) *error = message; }
QString toStoredPath(const Material& material, const QString& target, const QString& current) {
    if (current.trimmed().isEmpty()) return {};
    return QFileInfo(target).absoluteDir().relativeFilePath(material.resolvePath(current));
}
bool getString(const QJsonValue& value, QString& target) {
    if (!value.isString()) return false;
    target = value.toString(); return true;
}
bool getNumber(const QJsonValue& value, float& target) {
    if (!value.isDouble()) return false;
    const double n = value.toDouble();
    if (!std::isfinite(n)) return false;
    target = static_cast<float>(n); return std::isfinite(target);
}
bool getVec(const QJsonValue& value, QVector3D& target) {
    if (!value.isArray()) return false;
    const QJsonArray a = value.toArray();
    if (a.size() != 3) return false;
    float x, y, z;
    if (!getNumber(a[0], x) || !getNumber(a[1], y) || !getNumber(a[2], z)) return false;
    target = QVector3D(x, y, z); return true;
}
}

bool MaterialAsset::save(const Material& material, const QString& absolutePath, QString* error) {
    const QString full = QFileInfo(absolutePath).absoluteFilePath();
    if (!QDir().mkpath(QFileInfo(full).absolutePath())) {
        fail(error, "Cannot create material directory"); return false;
    }
    QSaveFile file(full);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        fail(error, "Cannot write material: " + file.errorString()); return false;
    }
    QTextStream out(&file);
    out << "# EldridgeEngine material asset (TOML v1)\nversion = 1\n";
    auto str = [&](const char* name, const QString& value) { out << name << " = " << quote(value) << '\n'; };
    auto f = [&](const char* name, float value) { out << name << " = " << number(value) << '\n'; };
    auto v = [&](const char* name, const QVector3D& value) { out << name << " = " << vec(value) << '\n'; };
    str("vertex_shader", toStoredPath(material, full, material.vert));
    str("fragment_shader", toStoredPath(material, full, material.frag));
    str("albedo_texture", toStoredPath(material, full, material.albedoTexture));
    str("specular_texture", toStoredPath(material, full, material.specularTexture));
    str("normal_texture", toStoredPath(material, full, material.normalTexture));
    str("metallic_texture", toStoredPath(material, full, material.metallicTexture));
    str("roughness_texture", toStoredPath(material, full, material.roughnessTexture));
    v("albedo", material.albedo);
    v("specular", material.specular);
    v("normal", material.normal);
    v("ambient", material.ambient);
    v("emissive", material.emissive);
    f("metallic", material.metallic);
    f("roughness", material.roughness);
    f("normal_strength", material.normalStrength);
    out.flush();
    if (out.status() != QTextStream::Ok || !file.commit()) {
        fail(error, "Could not finish saving .eldmat: " + file.errorString()); return false;
    }
    return true;
}

bool MaterialAsset::load(Material& destination, const QString& absolutePath, QString* error) {
    QFile file(absolutePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fail(error, "Cannot open material: " + file.errorString()); return false;
    }
    Material loaded;  // transactional: don't alter destination on failure
    QSet<QString> visited;
    QTextStream in(&file);
    int lineNo = 0;
    while (!in.atEnd()) {
        ++lineNo;
        const QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;
        const int equal = line.indexOf('=');
        if (equal < 1) { fail(error, QString("Line %1: expected key = value").arg(lineNo)); return false; }
        const QString key = line.left(equal).trimmed();
        const QString rawValue = line.mid(equal + 1).trimmed();
        if (visited.contains(key)) { fail(error, "Duplicate material key: " + key); return false; }
        visited.insert(key);
        QJsonParseError jsonError;
        const auto parsed = QJsonDocument::fromJson(("[" + rawValue + "]").toUtf8(), &jsonError);
        if (jsonError.error != QJsonParseError::NoError || !parsed.isArray() || parsed.array().size() != 1) {
            fail(error, QString("Line %1: unsupported value").arg(lineNo)); return false;
        }
        const QJsonValue value = parsed.array().at(0);
        bool ok = true;
        if (key == "version") ok = value.isDouble() && value.toInt(-1) == 1;
        else if (key == "vertex_shader") ok = getString(value, loaded.vert);
        else if (key == "fragment_shader") ok = getString(value, loaded.frag);
        else if (key == "albedo_texture") ok = getString(value, loaded.albedoTexture);
        else if (key == "specular_texture") ok = getString(value, loaded.specularTexture);
        else if (key == "normal_texture") ok = getString(value, loaded.normalTexture);
        else if (key == "metallic_texture") ok = getString(value, loaded.metallicTexture);
        else if (key == "roughness_texture") ok = getString(value, loaded.roughnessTexture);
        else if (key == "albedo") ok = getVec(value, loaded.albedo);
        else if (key == "specular") ok = getVec(value, loaded.specular);
        else if (key == "normal") ok = getVec(value, loaded.normal);
        else if (key == "ambient") ok = getVec(value, loaded.ambient);
        else if (key == "emissive") ok = getVec(value, loaded.emissive);
        else if (key == "metallic") ok = getNumber(value, loaded.metallic);
        else if (key == "roughness") ok = getNumber(value, loaded.roughness);
        else if (key == "normal_strength") ok = getNumber(value, loaded.normalStrength);
        // Unknown schema keys are not supported by this intentionally small parser.
        else ok = false;
        if (!ok) { fail(error, QString("Line %1: invalid or unknown key '%2'").arg(lineNo).arg(key)); return false; }
    }
    if (!visited.contains("version") || loaded.metallic < 0.f || loaded.metallic > 1.f ||
        loaded.roughness < 0.f || loaded.roughness > 1.f ||
        loaded.normalStrength < 0.f || loaded.normalStrength > 2.f ||
        loaded.vert.isEmpty() || loaded.frag.isEmpty()) {
        fail(error, "Missing/invalid version, shader path or metallic/roughness range"); return false;
    }
    loaded.assetPath = QFileInfo(absolutePath).absoluteFilePath();
    loaded.markDirty();
    destination = std::move(loaded);
    return true;
}

std::shared_ptr<Material> MaterialAsset::loadShared(const QString& path, QString* error) {
    const QString key = QFileInfo(path).absoluteFilePath();
    if (auto cached = assetCache().value(key).lock()) return cached;
    auto material = std::make_shared<Material>();
    if (!load(*material, key, error)) return {};
    assetCache().insert(key, material);
    return material;
}

void MaterialAsset::rememberShared(const std::shared_ptr<Material>& material) {
    if (!material || material->assetPath.isEmpty()) return;
    assetCache().insert(QFileInfo(material->assetPath).absoluteFilePath(), material);
}
