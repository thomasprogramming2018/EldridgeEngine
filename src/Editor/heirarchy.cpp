#include "heirarchy.h"
#include <QTreeWidgetItem>

#include <QMenu>
#include <QDropEvent>

#include <functional>

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


    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    setDragDropMode(QAbstractItemView::InternalMove);
}



void Hierarchy::dropEvent(QDropEvent* event)
{
    QTreeWidgetItem* dragged = currentItem();

    QTreeWidgetItem* target =
        itemAt(event->position().toPoint());

    if (!dragged || !target)
        return;

    uint64_t draggedId = dragged->data(0, Qt::UserRole).toULongLong();
    uint64_t targetId = target->data(0, Qt::UserRole).toULongLong();
    
    if (dropIndicatorPosition() != QAbstractItemView::OnItem)
    {
        event->ignore();
        return;
    }

    SceneObject* draggedObject = engine->findById(draggedId);
    SceneObject* targetObject = engine->findById(targetId);

    if (!draggedObject || !targetObject)
    {
        event->ignore();
        return;
    }

    draggedObject->setParent(targetObject);

    refresh();

    event->acceptProposedAction();


}


void Hierarchy::refresh()
{
    clear();

    std::function<void(SceneObject*, QTreeWidgetItem*)> addObject;

    addObject = [&](SceneObject* object, QTreeWidgetItem* parentItem)
    {
        QTreeWidgetItem* item;

        if (parentItem)
            item = new QTreeWidgetItem(parentItem);
        else
            item = new QTreeWidgetItem(this);

        item->setText(0, "SceneObject");

        uint64_t id = reinterpret_cast<uint64_t>(object);

        item->setData(
            0,
            Qt::UserRole,
            QVariant::fromValue(id)
        );

        for (SceneObject* child : object->children)
        {
            addObject(child, item);
        }
    };

    for (const auto& object : engine->objects)
    {
        if (object->parent == nullptr)
            addObject(object.get(), nullptr);
    }
}

