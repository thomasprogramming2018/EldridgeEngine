#pragma once
#include <QWidget>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QVector3D>
#include <memory>
#include "../Material.h"

class QFormLayout;

// Three scalar inputs for one QVector3D. No Qt meta-object required.
class Vector3Field : public QWidget {
public:
    explicit Vector3Field(QWidget* parent = nullptr);
    void setValue(const QVector3D& v);
    QVector3D value() const;
private:
    QDoubleSpinBox* x_ = nullptr;
    QDoubleSpinBox* y_ = nullptr;
    QDoubleSpinBox* z_ = nullptr;
};

class MaterialComponent : public QWidget {
    Q_OBJECT
public:
    explicit MaterialComponent(QWidget* parent = nullptr);
    void setMaterial(std::shared_ptr<Material> material);

signals:
    void materialApplied();
    // A new asset should not silently modify other objects using the old one.
    void materialReplaced(std::shared_ptr<Material> newMaterial);

private:
    std::shared_ptr<Material> currentMaterial;
    QWidget* editPanel = nullptr;
    QLineEdit* materialPathInput = nullptr;
    QLineEdit* vertPathInput = nullptr;
    QLineEdit* fragPathInput = nullptr;
    QLineEdit* albedoPathInput = nullptr;
    QLineEdit* specularPathInput = nullptr;
    QLineEdit* normalPathInput = nullptr;
    QLineEdit* metallicPathInput = nullptr;
    QLineEdit* roughnessPathInput = nullptr;
    QDoubleSpinBox* metallicInput = nullptr;
    QDoubleSpinBox* roughnessInput = nullptr;
    Vector3Field* albedoInput = nullptr;
    Vector3Field* specularInput = nullptr;
    Vector3Field* normalInput = nullptr;
    Vector3Field* ambientInput = nullptr;
    Vector3Field* emissiveInput = nullptr;

    QLineEdit* addPathField(QFormLayout* layout, const QString& title, const QString& filter);
    void fillFromMaterial();
    void copyFormTo(Material& target) const;
    void apply();
    void save();
    void saveAs();
    void loadAsset();
};
