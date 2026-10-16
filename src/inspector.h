#pragma once

#include <QWidget>
#include <QDockWidget>

#include "uiComponents/MaterialComponent.h"

class SceneObject;




class Inspector : public QDockWidget
{
    Q_OBJECT

public:
    explicit Inspector(QWidget* parent = nullptr);

    void setObject(SceneObject* object);

    MaterialComponent* materialComponent = nullptr;

private:
    SceneObject* selectedObject = nullptr;
};