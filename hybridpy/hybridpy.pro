TEMPLATE = lib
QT += core gui opengl

INCLUDEPATH += src
INCLUDEPATH += ../src

INCLUDEPATH += /usr/include/shiboken
INCLUDEPATH += /usr/include/PySide
INCLUDEPATH += /usr/include/PySide/QtCore
INCLUDEPATH += /usr/include/PySide/QtGui
QMAKE_CXXFLAGS += $$system(python-config --cflags)

LIBS += -ldl $$system(python-config --ldflags)
LIBS += -lpyside-python2.7
LIBS += -lshiboken-python2.7
LIBS += -L.. -lSunshine

TARGET = ../sunshine

SOURCES += \
    sunshine/sunshine_module_wrapper.cpp \
    sunshine/sunshine_wrapper.cpp \
