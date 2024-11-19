cmake_minimum_required(VERSION 3.28)
project(Quizzes LANGUAGES CXX)

include("cmake/qt.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Network
        NetworkAuth
        Concurrent
)

qt_standard_project_setup()

file(GLOB PROJECT_SOURCES "main.cpp" "include/*.h" "include/*.ui" "src/*.cpp" "resources/*.qrc")

qt_add_executable(Quizzes ${PROJECT_SOURCES})
target_include_directories(Quizzes PRIVATE "include")

# set_target_properties(Quizzes
#     PROPERTIES
#         WIN32_EXECUTABLE TRUE
# )

target_link_libraries(Quizzes
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::NetworkAuth
        Qt::Concurrent
)

target_compile_definitions(Quizzes PRIVATE DEVELOPMENT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")