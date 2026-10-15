#include "heirarchy.h"
#include <QTreeWidgetItem>

Hierarchy::Hierarchy(Engine* engine, QWidget* parent)
    : QTreeWidget(parent), engine(engine)
{
    setHeaderLabel("Scene Hierarchy");
}

void Hierarchy::refresh()
{
    clear();

    for (const auto& object : engine->objects)
    {
        auto* item = new QTreeWidgetItem(this);
        item->setText(0, "SceneObject");
    }
}