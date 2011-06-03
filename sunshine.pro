# -------------------------------------------------
# Project created by QtCreator 2011-04-30T08:27:50
# -------------------------------------------------
QT += opengl \
    script \
    webkit
LIBS += -lGLEW
TARGET = sunshine
TEMPLATE = app
SOURCES += main.cpp \
    sunshine.cpp \
    panelgl.cpp \
    shader.cpp \
    camera.cpp \
    util.cpp \
    geometry.cpp \
    register.cpp
HEADERS += sunshine.h \
    panelgl.h \
    shader.h \
    util.h \
    camera.h \
    geometry.h \
    register.h
FORMS += sunshine.ui
