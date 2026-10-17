#include "MaterialComponent.h"
#include "../MaterialAsset.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <utility>

namespace {

void setSpinBoxValue(QDoubleSpinBox* spinBox, double value) {
    if (spinBox) {
        spinBox->setValue(value);
    }
}

void setPathValue(QLineEdit* input, const QString& value) {
    if (input) {
        input->setText(value);
    }
}

void clearTextFields(const std::initializer_list<QLineEdit*>& inputs) {
    for (QLineEdit* input : inputs) {
        if (input) {
            input->clear();
        }
    }
}

} // namespace

Vector3Field::Vector3Field(QWidget* parent) : QWidget(parent) {
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);

    for (auto** input : { &x_, &y_, &z_ }) {
        *input = new QDoubleSpinBox(this);
        (*input)->setRange(-10000.0, 10000.0);
        (*input)->setDecimals(3);
        (*input)->setSingleStep(0.05);
        row->addWidget(*input);
    }
}

void Vector3Field::setValue(const QVector3D& v) {
    x_->setValue(v.x());
    y_->setValue(v.y());
    z_->setValue(v.z());
}

QVector3D Vector3Field::value() const {
    return QVector3D(x_->value(), y_->value(), z_->value());
}

MaterialComponent::MaterialComponent(QWidget* parent) : QWidget(parent) {
    auto* outer = new QVBoxLayout(this);
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);

    editPanel = new QWidget(scroll);
    auto* form = new QFormLayout(editPanel);

    configurePathFields(form);
    configureScalarFields(form);
    configureVectorFields(form);

    scroll->setWidget(editPanel);
    outer->addWidget(scroll);

    auto* actions = new QHBoxLayout();
    auto* applyButton = new QPushButton("Apply", this);
    auto* saveButton = new QPushButton("Save", this);
    auto* saveAsButton = new QPushButton("Save As", this);

    actions->addWidget(applyButton);
    actions->addWidget(saveButton);
    actions->addWidget(saveAsButton);
    outer->addLayout(actions);

    connect(applyButton, &QPushButton::clicked, this, &MaterialComponent::apply);
    connect(saveButton, &QPushButton::clicked, this, &MaterialComponent::save);
    connect(saveAsButton, &QPushButton::clicked, this, &MaterialComponent::saveAs);

    setMaterial(nullptr);
}

QLineEdit* MaterialComponent::addPathField(
    QFormLayout* form,
    const QString& title,
    const QString& filter)
{
    auto* row = new QHBoxLayout();
    auto* input = new QLineEdit(editPanel);
    auto* browse = new QPushButton("...", editPanel);
    browse->setFixedWidth(35);

    row->addWidget(input, 1);
    row->addWidget(browse);
    form->addRow(title, row);

    connect(browse, &QPushButton::clicked, this, [this, input, filter]() {
        const QString picked = QFileDialog::getOpenFileName(this, "Select File", input->text(), filter);
        if (!picked.isEmpty()) {
            input->setText(picked);
        }
    });

    return input;
}

void MaterialComponent::configurePathFields(QFormLayout* form) {
    materialPathInput = new QLineEdit(editPanel);
    materialPathInput->setReadOnly(true);
    form->addRow("Material Asset", materialPathInput);

    auto* loadButton = new QPushButton("Load .eldmat", editPanel);
    form->addRow(loadButton);
    connect(loadButton, &QPushButton::clicked, this, &MaterialComponent::loadAsset);

    vertPathInput = addPathField(form, "Vertex Shader", "GLSL (*.vert)");
    fragPathInput = addPathField(form, "Fragment Shader", "GLSL (*.frag)");
    albedoPathInput = addPathField(form, "Albedo Texture", "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
    specularPathInput = addPathField(form, "Specular Texture", "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
    normalPathInput = addPathField(form, "Normal Texture", "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
    metallicPathInput = addPathField(form, "Metallic Texture", "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
    roughnessPathInput = addPathField(form, "Roughness Texture", "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
}

void MaterialComponent::configureScalarFields(QFormLayout* form) {
    metallicInput = new QDoubleSpinBox(editPanel);
    metallicInput->setRange(0.0, 1.0);
    metallicInput->setDecimals(3);
    metallicInput->setSingleStep(0.05);

    roughnessInput = new QDoubleSpinBox(editPanel);
    roughnessInput->setRange(0.0, 1.0);
    roughnessInput->setDecimals(3);
    roughnessInput->setSingleStep(0.05);

    form->addRow("Metallic", metallicInput);
    form->addRow("Roughness", roughnessInput);

    normalStrengthInput = new QDoubleSpinBox(editPanel);
    normalStrengthInput->setRange(0.0, 2.0);
    normalStrengthInput->setDecimals(2);
    normalStrengthInput->setSingleStep(0.1);
    form->addRow("Normal Strength", normalStrengthInput);
}

void MaterialComponent::configureVectorFields(QFormLayout* form) {
    albedoInput = new Vector3Field(editPanel);
    specularInput = new Vector3Field(editPanel);
    normalInput = new Vector3Field(editPanel);
    ambientInput = new Vector3Field(editPanel);
    emissiveInput = new Vector3Field(editPanel);

    form->addRow("Albedo RGB", albedoInput);
    form->addRow("Specular RGB", specularInput);
    form->addRow("Normal Direction XYZ", normalInput);
    form->addRow("Ambient RGB", ambientInput);
    form->addRow("Emissive RGB", emissiveInput);
}

void MaterialComponent::setMaterial(std::shared_ptr<Material> material) {
    currentMaterial = std::move(material);
    editPanel->setEnabled(static_cast<bool>(currentMaterial));
    setPathValue(materialPathInput, currentMaterial ? currentMaterial->assetPath : QString());
    fillFromMaterial();
}

void MaterialComponent::clearForm() {
    clearTextFields({ vertPathInput, fragPathInput, albedoPathInput,
                      specularPathInput, normalPathInput,
                      metallicPathInput, roughnessPathInput });

    setSpinBoxValue(metallicInput, 0.0);
    setSpinBoxValue(roughnessInput, 0.0);
    setSpinBoxValue(normalStrengthInput, 1.5);

    if (albedoInput) albedoInput->setValue(QVector3D());
    if (specularInput) specularInput->setValue(QVector3D());
    if (normalInput) normalInput->setValue(QVector3D());
    if (ambientInput) ambientInput->setValue(QVector3D());
    if (emissiveInput) emissiveInput->setValue(QVector3D());
}

void MaterialComponent::fillFromMaterial() {
    if (!currentMaterial) {
        clearForm();
        return;
    }

    fillFormFromMaterial(*currentMaterial);
}

void MaterialComponent::fillFormFromMaterial(const Material& material) {
    setPathValue(vertPathInput, material.resolvePath(material.vert));
    setPathValue(fragPathInput, material.resolvePath(material.frag));
    setPathValue(albedoPathInput, material.resolvePath(material.albedoTexture));
    setPathValue(specularPathInput, material.resolvePath(material.specularTexture));
    setPathValue(normalPathInput, material.resolvePath(material.normalTexture));
    setPathValue(metallicPathInput, material.resolvePath(material.metallicTexture));
    setPathValue(roughnessPathInput, material.resolvePath(material.roughnessTexture));

    setSpinBoxValue(metallicInput, material.metallic);
    setSpinBoxValue(roughnessInput, material.roughness);
    setSpinBoxValue(normalStrengthInput, material.normalStrength);

    if (albedoInput) albedoInput->setValue(material.albedo);
    if (specularInput) specularInput->setValue(material.specular);
    if (normalInput) normalInput->setValue(material.normal);
    if (ambientInput) ambientInput->setValue(material.ambient);
    if (emissiveInput) emissiveInput->setValue(material.emissive);
}

QString MaterialComponent::toAssetRelativePath(const Material& material, const QString& rawPath) {
    const QString trimmed = rawPath.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }

    const QDir assetDir = material.assetPath.isEmpty()
        ? QDir::current()
        : QFileInfo(material.assetPath).absoluteDir();

    return QFileInfo(trimmed).isAbsolute() ? assetDir.relativeFilePath(trimmed) : trimmed;
}

void MaterialComponent::copyFormTo(Material& material) const {
    material.vert = toAssetRelativePath(material, vertPathInput->text());
    material.frag = toAssetRelativePath(material, fragPathInput->text());
    material.albedoTexture = toAssetRelativePath(material, albedoPathInput->text());
    material.specularTexture = toAssetRelativePath(material, specularPathInput->text());
    material.normalTexture = toAssetRelativePath(material, normalPathInput->text());
    material.metallicTexture = toAssetRelativePath(material, metallicPathInput->text());
    material.roughnessTexture = toAssetRelativePath(material, roughnessPathInput->text());

    material.metallic = static_cast<float>(metallicInput->value());
    material.roughness = static_cast<float>(roughnessInput->value());
    material.normalStrength = static_cast<float>(normalStrengthInput->value());
    material.albedo = albedoInput->value();
    material.specular = specularInput->value();
    material.normal = normalInput->value();
    material.ambient = ambientInput->value();
    material.emissive = emissiveInput->value();
}

void MaterialComponent::apply() {
    if (!currentMaterial) {
        return;
    }

    copyFormTo(*currentMaterial);
    currentMaterial->markDirty();
    emit materialApplied();
}

void MaterialComponent::save() {
    if (!currentMaterial) {
        return;
    }

    if (currentMaterial->assetPath.isEmpty()) {
        saveAs();
        return;
    }

    apply();

    QString error;
    if (!MaterialAsset::save(*currentMaterial, currentMaterial->assetPath, &error)) {
        QMessageBox::warning(this, "Material Save Failed", error);
    }
}

void MaterialComponent::saveAs() {
    if (!currentMaterial) {
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this,
        "Save Material As",
        "materials/NewMaterial.eldmat",
        "Eldridge Materials (*.eldmat)");

    if (path.isEmpty()) {
        return;
    }

    if (QFileInfo(path).exists()) {
        QMessageBox::warning(this, "Save As", "Choose a new filename to preserve the original asset.");
        return;
    }

    Material draft = *currentMaterial;
    copyFormTo(draft);

    QString error;
    if (!MaterialAsset::save(draft, path, &error)) {
        QMessageBox::warning(this, "Material Save Failed", error);
        return;
    }

    auto fresh = std::make_shared<Material>();
    if (!MaterialAsset::load(*fresh, path, &error)) {
        QMessageBox::warning(this, "Material Reload Failed", error);
        return;
    }

    MaterialAsset::rememberShared(fresh);
    emit materialReplaced(fresh);
    setMaterial(fresh);
    emit materialApplied();
}

void MaterialComponent::loadAsset() {
    if (!currentMaterial) {
        return;
    }

    const QString path = QFileDialog::getOpenFileName(
        this,
        "Load Material",
        QString(),
        "Eldridge Materials (*.eldmat)");

    if (path.isEmpty()) {
        return;
    }

    QString error;
    auto fresh = MaterialAsset::loadShared(path, &error);
    if (!fresh) {
        QMessageBox::warning(this, "Material Load Failed", error);
        return;
    }

    emit materialReplaced(fresh);
    setMaterial(fresh);
    emit materialApplied();
}
