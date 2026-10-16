#include "inspector.h"
#include "SceneObject.h"
#include <memory>
#include <utility>

Inspector::Inspector(QWidget* parent) : QDockWidget("Inspector", parent) {
    materialComponent = new MaterialComponent(this);
    setWidget(materialComponent);
    connect(materialComponent, &MaterialComponent::materialReplaced,
            this, [this](std::shared_ptr<Material> newMaterial) {
        if (selectedObject) selectedObject->material = std::move(newMaterial);
    });
}

void Inspector::setObject(SceneObject* object) {
    selectedObject = object;
    // IMPORTANT: also clears the previous shared_ptr when there is no selection.
    materialComponent->setMaterial(selectedObject ? selectedObject->material : nullptr);
}
