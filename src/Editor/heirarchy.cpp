#include "heirarchy.h"
#include <QTreeWidgetItem>

#include <QMenu>

Hierarchy::Hierarchy(Engine* engine, QWidget* parent)
    : QTreeWidget(parent), engine(engine)
{
    setHeaderLabel("Scene Hierarchy");

    refresh();


    connect(this, &QTreeWidget::itemSelectionChanged,
        this, [this]()
    {
        QTreeWidgetItem* item = currentItem();
        if (!item) return;
    
        uint64_t id = item->data(0, Qt::UserRole).toULongLong();
    
        emit objectSelected(id);
    });

    connect(engine, &Engine::objectCreated, this, [this]() {
        this->refresh();
    });


    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::customContextMenuRequested,
            this, [this](const QPoint& pos)
    {
        QTreeWidgetItem* item = itemAt(pos);
        if (!item) return;

        QMenu menu;
        QAction* remove = menu.addAction("Delete");

        if (menu.exec(viewport()->mapToGlobal(pos)) == remove)
        {
            uint64_t id = item->data(0, Qt::UserRole).toULongLong();
            emit deleteRequested(id);
        }
    });
}

void Hierarchy::refresh()
{
    clear();

    for (const auto& object : engine->objects)
    {
        auto* item = new QTreeWidgetItem(this);

        item->setText(0, "SceneObject");

        uint64_t id = reinterpret_cast<uint64_t>(object.get());

        item->setData(0, Qt::UserRole,
                      QVariant::fromValue(id));
    }
}


