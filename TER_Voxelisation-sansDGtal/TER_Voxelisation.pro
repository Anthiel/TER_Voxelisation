QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = starterLight
TEMPLATE = app


DEFINES += QT_DEPRECATED_WARNINGS


unix:!macx {
    LIBS += -lglut -lGLU
    LIBS += -L$$PWD/../OpenMesh/liblinux/ -lOpenMeshCore
    LIBS += -lDGtal

    INCLUDEPATH += $$PWD/../OpenMesh/inc/
    DEPENDPATH += $$PWD/../OpenMesh/inc/
    DEPENDPATH += $$PWD/../OpenMesh/liblinux/
}

macx: {
    INCLUDEPATH += $$PWD/../OpenMesh/inc/
    LIBS += -L$$PWD/../OpenMesh/libosx/ -lOpenMeshCore -lOpenMeshTools
    LIBS += -lDGtal
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        meshviewerwidget.cpp \
        Space.cpp

HEADERS += \
        mainwindow.h \
        meshviewerwidget.h \
        Space.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    ressources.qrc


