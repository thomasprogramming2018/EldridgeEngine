#include "MaterialComponent.h"
#include "../MaterialAsset.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <utility>

Vector3Field::Vector3Field(QWidget* parent) : QWidget(parent) {
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);
    for (auto** input : {&x_, &y_, &z_}) {
        *input = new QDoubleSpinBox(this);
        (*input)->setRange(-10000.0, 10000.0);
        (*input)->setDecimals(3);
        (*input)->setSingleStep(0.05);
        row->addWidget(*input);
    }
}
void Vector3Field::setValue(const QVector3D& v) {
    x_->setValue(v.x()); y_->setValue(v.y()); z_->setValue(v.z());
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

    // The asset path is loaded with a dedicated Load button.
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

    metallicInput = new QDoubleSpinBox(editPanel);
    metallicInput->setRange(0, 1); metallicInput->setDecimals(3);
    metallicInput->setSingleStep(0.05);
    roughnessInput = new QDoubleSpinBox(editPanel);
    roughnessInput->setRange(0, 1); roughnessInput->setDecimals(3);
    roughnessInput->setSingleStep(0.05);
    form->addRow("Metallic", metallicInput);
    form->addRow("Roughness", roughnessInput);

    albedoInput = new Vector3Field(editPanel);
    specularInput = new Vector3Field(editPanel);
    normalInput = new Vector3Field(editPanel);
    ambientInput = new Vector3Field(editPanel);
    emissiveInput = new Vector3Field(editPanel);
    form->addRow("Albedo RGB", albedoInput);
    form->addRow("Specular RGB", specularInput);
    form->addRow("Normal XYZ", normalInput);
    form->addRow("Ambient RGB", ambientInput);
    form->addRow("Emissive RGB", emissiveInput);

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
    // If nothing is selected, don't let a stale UI edit a material.
    setMaterial(nullptr);
}

QLineEdit* MaterialComponent::addPathField(
    QFormLayout* form, const QString& title, const QString& filter)
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
        if (!picked.isEmpty()) input->setText(picked);
    });
    return input;
}

void MaterialComponent::setMaterial(std::shared_ptr<Material> material) {
    currentMaterial = std::move(material);
    editPanel->setEnabled(static_cast<bool>(currentMaterial));
    materialPathInput->setText(currentMaterial ? currentMaterial->assetPath : QString());
    fillFromMaterial();
}

void MaterialComponent::fillFromMaterial() {
    if (!currentMaterial) {
        for (auto* input : {vertPathInput, fragPathInput, albedoPathInput,
                            specularPathInput, normalPathInput, metallicPathInput,
                            roughnessPathInput}) input->clear();
        metallicInput->setValue(0); roughnessInput->setValue(0);
        albedoInput->setValue(QVector3D());
        specularInput->setValue(QVector3D());
        normalInput->setValue(QVector3D());
        ambientInput->setValue(QVector3D());
        emissiveInput->setValue(QVector3D());
        return;
    }
    const auto& m = *currentMaterial;
    vertPathInput->setText(m.resolvePath(m.vert));
    fragPathInput->setText(m.resolvePath(m.frag));
    albedoPathInput->setText(m.resolvePath(m.albedoTexture));
    specularPathInput->setText(m.resolvePath(m.specularTexture));
    normalPathInput->setText(m.resolvePath(m.normalTexture));
    metallicPathInput->setText(m.resolvePath(m.metallicTexture));
    roughnessPathInput->setText(m.resolvePath(m.roughnessTexture));
    metallicInput->setValue(m.metallic);
    roughnessInput->setValue(m.roughness);
    albedoInput->setValue(m.albedo);
    specularInput->setValue(m.specular);
    normalInput->setValue(m.normal);
    ambientInput->setValue(m.ambient);
    emissiveInput->setValue(m.emissive);
}

void MaterialComponent::copyFormTo(Material& m) const {
    // Editor shows absolute paths; stored material paths are relative to its asset.
    const QDir base = m.assetPath.isEmpty() ? QDir::current() : QFileInfo(m.assetPath).absoluteDir();
    auto store = [&](QLineEdit* input) -> QString {
        const QString raw = input->text().trimmed();
        if (raw.isEmpty()) return {};
        return QFileInfo(raw).isAbsolute() ? base.relativeFilePath(raw) : raw;
    };
    m.vert = store(vertPathInput);
    m.frag = store(fragPathInput);
    m.albedoTexture = store(albedoPathInput);
    m.specularTexture = store(specularPathInput);
    m.normalTexture = store(normalPathInput);
    m.metallicTexture = store(metallicPathInput);
    m.roughnessTexture = store(roughnessPathInput);
    m.metallic = static_cast<float>(metallicInput->value());
    m.roughness = static_cast<float>(roughnessInput->value());
    m.albedo = albedoInput->value();
    m.specular = specularInput->value();
    m.normal = normalInput->value();
    m.ambient = ambientInput->value();
    m.emissive = emissiveInput->value();
}

void MaterialComponent::apply() {
    if (!currentMaterial) return;
    copyFormTo(*currentMaterial);
    currentMaterial->markDirty();
    emit materialApplied();
}
void MaterialComponent::save() {
    if (!currentMaterial) return;
    if (currentMaterial->assetPath.isEmpty()) { saveAs(); return; }
    apply();
    QString error;
    if (!MaterialAsset::save(*currentMaterial, currentMaterial->assetPath, &error))
        QMessageBox::warning(this, "Material Save Failed", error);
}
void MaterialComponent::saveAs() {
    if (!currentMaterial) return;
    const QString path = QFileDialog::getSaveFileName(
        this, "Save Material As", "materials/NewMaterial.eldmat", "Eldridge Materials (*.eldmat)");
    if (path.isEmpty()) return;
    if (QFileInfo(path).exists()) {
        QMessageBox::warning(this, "Save As", "Choose a new filename to preserve the original asset.");
        return;
    }
    Material draft = *currentMaterial;
    copyFormTo(draft);
    QString error;
    if (!MaterialAsset::save(draft, path, &error)) {
        QMessageBox::warning(this, "Material Save Failed", error); return;
    }
    // Reload rebased paths from the new file; old shared material is unchanged.
    auto fresh = std::make_shared<Material>();
    if (!MaterialAsset::load(*fresh, path, &error)) {
        QMessageBox::warning(this, "Material Reload Failed", error); return;
    }
    MaterialAsset::rememberShared(fresh);
    emit materialReplaced(fresh);
    setMaterial(fresh);
    emit materialApplied();
}
void MaterialComponent::loadAsset() {
    if (!currentMaterial) return;
    const QString path = QFileDialog::getOpenFileName(
        this, "Load Material", QString(), "Eldridge Materials (*.eldmat)");
    if (path.isEmpty()) return;
    QString error;
    auto fresh = MaterialAsset::loadShared(path, &error);
    if (!fresh) {
        QMessageBox::warning(this, "Material Load Failed", error); return;
    }
    emit materialReplaced(fresh);
    setMaterial(fresh);
    emit materialApplied();
}
