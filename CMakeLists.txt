set(FORM_H
    Headers/MainWindow.h

)

set(SOURCES
  Sources/main.cpp
  Sources/MainWindow.cpp
)

set(FORMS
  Forms/MainWindow.ui
)



################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(TER_Voxelisation)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)

qt5_wrap_ui(FORM_H ${FORMS})

# Tell CMake to create the helloworld executable
add_executable(${PROJECT_NAME} ${SOURCES} ${FORM_H})

#Required in DGtal
FIND_PACKAGE(DGtal REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DGTAL_LIBRARIES})
# Use the Widgets module from Qt 5.
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

