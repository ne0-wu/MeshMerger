cmake_minimum_required(VERSION 3.25)

project(MeshMerger)

set(CMAKE_CXX_STANDARD 20)

find_package(Eigen3 CONFIG REQUIRED)
find_package(OpenMesh CONFIG REQUIRED)

add_compile_definitions(_USE_MATH_DEFINES)

add_subdirectory(MyGL)

set(HEADERS
    Mesh.h
    MeshToGL.h
    Dijkstra.h
)

set(SOURCES
    main.cpp
    Dijkstra.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    OpenMeshCore
    MyGL
)

set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
if(EXISTS ${DATA_DIR} AND IS_DIRECTORY ${DATA_DIR})
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DATA_DIR}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
        COMMENT "Copying data directory to output directory"
    )
endif()