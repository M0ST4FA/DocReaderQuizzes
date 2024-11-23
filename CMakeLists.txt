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

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.rc)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.rc PROPERTIES LANGUAGE RC)
endif()

# MAIN APP
file(GLOB PROJECT_SOURCES "main.cpp" "include/*.h" "include/*.ui" "src/*.cpp")
# QT_ADD_RESOURCES(PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.qrc")

list(REMOVE_ITEM PROJECT_SOURCES
"C:/Users/mosta/Desktop/Projects/Programming/Apps/DocReaderQuizzes/src/lexer.cpp")

message(${PROJECT_SOURCES})

qt_add_executable(Quizzes ${PROJECT_SOURCES} ${WIN32_RESOURCES})
target_include_directories(Quizzes PRIVATE "include")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set_target_properties(Quizzes
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
endif()

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
target_compile_definitions(Quizzes PRIVATE CLIENT_ID="${CLIENT_ID}")
target_compile_definitions(Quizzes PRIVATE CLIENT_SECRET="${CLIENT_SECRET}")

# Create a new assets directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/icons)

# Copy specific files from assets/icons to the new assets directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/appicon.ico DESTINATION ${CMAKE_BINARY_DIR}/assets/icons)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/appicon.svg DESTINATION ${CMAKE_BINARY_DIR}/assets/icons)

install(TARGETS Quizzes DESTINATION bin)
install(DIRECTORY ${CMAKE_BINARY_DIR}/assets DESTINATION assets)
