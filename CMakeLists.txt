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
        LinguistTools
)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES ar_EG)
set(CMAKE_AUTORCC ON)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.rc)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.rc PROPERTIES LANGUAGE RC)
endif()

# MAIN APP
file(GLOB PROJECT_SOURCES "main.cpp" "include/*.h" "include/*.ui" "src/*.cpp")
qt_add_resources(PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.qrc")

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
    target_compile_definitions(Quizzes PRIVATE QT_NO_DEBUG_OUTPUT)
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

# Compile definitions
target_compile_definitions(Quizzes PRIVATE DEVELOPMENT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(Quizzes PRIVATE CLIENT_ID="${CLIENT_ID}")
target_compile_definitions(Quizzes PRIVATE CLIENT_SECRET="${CLIENT_SECRET}")
target_compile_definitions(Quizzes PRIVATE GEMINI_API_KEY="${GEMINI_API_KEY}")

# Create a new assets directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/icons)

# Copy specific files from assets/icons to the new assets directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/appicon.ico DESTINATION ${CMAKE_BINARY_DIR}/assets/icons)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/appicon.svg DESTINATION ${CMAKE_BINARY_DIR}/assets/icons)

install(TARGETS Quizzes DESTINATION bin)
install(DIRECTORY ${CMAKE_BINARY_DIR}/assets DESTINATION assets)

qt_add_lupdate(
    SOURCE_TARGETS Quizzes
    TS_FILES ${PROJECT_NAME}_ar_EG.ts
)