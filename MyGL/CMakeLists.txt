cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

set(REQUIRED_PACKAGES
    glad
    glm
    glfw3
    imgui
)

foreach(package ${REQUIRED_PACKAGES})
    find_package(${package} CONFIG REQUIRED)
endforeach()

set(MYGL_HEADERS
    Window.h
    Camera.h
    Shader.h
    Mesh.h
    PointCloud.h
    LineSegment.h
    PickVertex.h
    LogConsole.h
    Utils.h
)

set(MYGL_SOURCES
    Window.cpp
    Camera.cpp
    Shader.cpp
    Mesh.cpp
    PointCloud.cpp
    LineSegment.cpp
    PickVertex.cpp
    LogConsole.cpp
)

add_library(MyGL
    ${MYGL_HEADERS}
    ${MYGL_SOURCES}
)

source_group("Header Files" FILES ${MYGL_HEADERS})
source_group("Source Files" FILES ${MYGL_SOURCES})

target_include_directories(MyGL
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MyGL
	PUBLIC
		glad::glad
		glm::glm
		glfw
		imgui::imgui
)