#pragma once

#include <QTreeWidget>
#include "../Engine.h"

class Hierarchy : public QTreeWidget
{
public:
    explicit Hierarchy(Engine* engine, QWidget* parent = nullptr);

    void refresh();

private:
    Engine* engine; // Non-owning pointer
};