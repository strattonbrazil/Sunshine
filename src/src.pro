# -------------------------------------------------
# Project created by QtCreator 2011-04-30T08:27:50
# -------------------------------------------------
QT += opengl \
    script \
    webkit
QMAKE_CXXFLAGS += $$system(python-config --cflags)
LIBS += -lGLEW
LIBS += -laqsis_core
LIBS += -lPythonQt
LIBS += -lPythonQt_QtAll
QMAKE_LIBS += $$system(python-config --libs)
TARGET = ../sunshine
TEMPLATE = app 
SOURCES += main.cpp \
    sunshine.cpp \
    panelgl.cpp \
    shader.cpp \
    camera.cpp \
    util.cpp \
    geometry.cpp \
    register.cpp \
    primitive.cpp \
    renderwidget.cpp \
    imageviewer.cpp \
    settings.cpp \
    $$files(./qtpropertybrowser/*.cpp) \
    select.cpp \
    face_util.cpp \
    project_util.cpp \
    vertex_util.cpp
HEADERS += sunshine.h \
    panelgl.h \
    shader.h \
    util.h \
    camera.h \
    geometry.h \
    register.h \
    primitive.h \
    renderwidget.h \
    imageviewer.h \
    settings.h \
    $$files(./qtpropertybrowser/*.h) \
    select.h \
    face_util.h \
    project_util.h \
    vertex_util.h
FORMS += sunshine.ui \
    renderwidget.ui
