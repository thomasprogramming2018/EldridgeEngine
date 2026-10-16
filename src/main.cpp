#include <QApplication>
#include <QMainWindow>

#include "Renderer.h"

#include <QDockWidget>
#include <QLabel>

#include <QElapsedTimer>
#include <QTimer>
#include <QDebug>

#include <QPushButton>

#include "Engine.h"

#include <QWidget>

#include <QVBoxLayout>


#include "Editor/heirarchy.h"

#include "inspector.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QElapsedTimer clock;
    clock.start();

    QTimer timer;


    QDockWidget *heirarchy = new QDockWidget("heirarchy");

    QMainWindow window;

    Inspector* inspector2 = new Inspector(&window);

    window.addDockWidget(
        Qt::RightDockWidgetArea,
        inspector2
    );


    Engine engine;

    Renderer *viewport = new Renderer(&engine);


    std::vector<Buffer> buffers;



    QObject::connect(&timer, &QTimer::timeout, [&]()
                     {
                         float dt = clock.restart() / 1000.0f;

                         engine.update(dt);  // Simulation
                         viewport->update(); // Request rendering
                         viewport->tick(dt);


                     });

    timer.start(16);
    
    
    QWidget *panel = new QWidget();

    
    QVBoxLayout *layout = new QVBoxLayout();
    
    QPushButton *createCube = new QPushButton();


    QVBoxLayout *heirPanel = new QVBoxLayout();


    layout->addWidget(new QLabel("SceneObjects"));

    layout->addWidget(createCube);

    layout->addStretch();







    

        
    panel->setLayout(layout);
    heirarchy->setWidget(panel);

    

    
    

    


    Hierarchy* hierarchy2 = new Hierarchy(&engine);

    layout->addWidget(hierarchy2);

    createCube->setText("create Cube");

    viewport->update();

    hierarchy2->refresh();


    QObject::connect(createCube, &QPushButton::clicked, &window, [&engine, buffers, viewport, hierarchy2]()
    {
        engine.createObject(MaterialType::PBR, buffers);
        viewport->update(); 



        hierarchy2->refresh();
    });


    window.addDockWidget(
        Qt::LeftDockWidgetArea,
        heirarchy);


 
    QObject::connect(
        hierarchy2,
        &Hierarchy::deleteRequested,
        &window,
        [&](uint64_t id)
        {
            inspector2->setObject(nullptr);  // Clear BEFORE destroying SceneObject
            viewport->releaseObjectTextures(id);
            engine.deleteObject(id);
            hierarchy2->refresh();
            viewport->update();

        }
    );
    

    QObject::connect(
        hierarchy2,
        &Hierarchy::objectSelected,
        &window,
        [&](uint64_t id)
        {
            engine.selectById(id);

            inspector2->setObject(engine.selectedObject);

            viewport->update();
        }
    );



    QObject::connect(viewport, &Renderer::selectionChanged, &window, [&]() {
        inspector2->setObject(engine.selectedObject);
    });

    window.setCentralWidget(viewport);
    window.resize(1200, 800);
    window.show();

    return app.exec();
}