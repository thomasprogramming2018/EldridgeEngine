#pragma once

#include <QTreeWidget>
#include "../Engine.h"

class Hierarchy : public QTreeWidget
{
    Q_OBJECT
public:
    explicit Hierarchy(Engine* engine, QWidget* parent = nullptr);

    void refresh();

    void selectById(uint64_t objectId)
    {
        engine->selectById(objectId); // Clear the selected object in the engine
    }

private:
    Engine* engine; // Non-owning pointer
signals:
    void deleteRequested(uint64_t objectId);
    
    void objectSelected(uint64_t objectId);
};