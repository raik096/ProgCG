cmake_minimum_required(VERSION 3.5.0)
project(ProgCG VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(SOURCES )


add_executable(ProgCG main.cpp)

target_sources(ProgCG PRIVATE

    includes/Mesh.cpp
    includes/BoundingBox.cpp
    includes/Renderable.cpp

    includes/Shader.cpp
    includes/Texture.cpp
    includes/FrameBuffer.cpp

    includes/Camera.cpp
    includes/Scene.cpp
    includes/Ligth.cpp

    includes/ModelLoader.cpp
    includes/Input.cpp

    includes/Color.cpp
    includes/materials/Material.cpp
    includes/materials/UnlitMaterial.cpp
    includes/materials/LitMaterial.cpp
    includes/materials/TerrainMaterial.cpp

    includes/Trackball.cpp
    includes/Orbit.cpp
    includes/Intersection.cpp

    includes/Carosello.cpp
    includes/Terrain.cpp
    includes/Track.cpp
)

target_sources(ProgCG PRIVATE
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui.cpp

    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    
    vendor/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(ProgCG PRIVATE 
    incldues
    includes/materials
    
    vendor
    vendor/imgui
    vendor/imgui/backends
    vendor/imgui/misc
    vendor/imgui/misc/cpp
    vendor/imgui/misc/fonts
    vendor/nanosvg/src
)

target_link_libraries(ProgCG GLEW glfw GL)

set_property(TARGET ProgCG PROPERTY CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_PROGRAM_PATH})
