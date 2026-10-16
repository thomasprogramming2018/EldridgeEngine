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

    if (!dragged || event->source() != this)
    {
        event->ignore();
        return;
    }

    uint64_t draggedId =
        dragged->data(0, Qt::UserRole).toULongLong();

    SceneObject* draggedObject = engine->findById(draggedId);

    if (!draggedObject)
    {
        event->ignore();
        return;
    }

    SceneObject* newParent = nullptr;

    auto position = dropIndicatorPosition();

    if (position != QAbstractItemView::OnViewport)
    {
        QTreeWidgetItem* target =
            itemAt(event->position().toPoint());

        if (!target)
        {
            event->ignore();
            return;
        }

        QTreeWidgetItem* parentItem = nullptr;

        if (position == QAbstractItemView::OnItem)
        {
            parentItem = target;
        }
        else
        {
            // AboveItem / BelowItem:
            // Become a sibling of the target.
            parentItem = target->parent();
        }

        if (parentItem)
        {
            uint64_t id =
                parentItem->data(0, Qt::UserRole).toULongLong();

            newParent = engine->findById(id);

            if (!newParent)
            {
                event->ignore();
                return;
            }
        }
    }

    // nullptr means scene root.
    draggedObject->setParent(newParent);

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

