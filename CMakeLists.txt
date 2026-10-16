cmake_minimum_required(VERSION 3.16)

project(EldridgeEngineLearning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    OpenGLWidgets
)



add_executable(EldridgeEngine
    src/main.cpp
    src/Engine.cpp
    src/Renderer.cpp
    src/Renderer.h
    src/SceneObject.cpp
    src/Shader.cpp
    src/Camera.cpp

    src/ResourceManager.cpp  # ADD THIS

    src/Editor/heirarchy.cpp

    

    
    src/Material.cpp

    src/uiComponents/MaterialComponent.cpp


    
    src/inspector.cpp

    src/MaterialAsset.cpp
)

target_link_libraries(EldridgeEngine PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
)