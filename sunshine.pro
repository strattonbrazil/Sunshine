######################################################################
# Automatically generated by qmake (2.01a) Thu Aug 11 06:41:12 2011
######################################################################

TEMPLATE = app
TARGET = sunshine
DEPENDPATH += . src
INCLUDEPATH += . src
QMAKE_CXXFLAGS += $$system(python-config --cflags)
QMAKE_LIBS += $$system(python-config --libs)
QT += opengl script
LIBS += -lGLEW -lGLU
LIBS += -laqsis_core
LIBS += -lboost_python
LIBS += -lassimp
#LIBS += -lCGAL
#LIBS += -lCGAL_Core
#LIBS += -lCGAL_PDB
#load(qttest_p4)

# Input
HEADERS += src/camera.h \
           src/face_util.h \
           src/geometry.h \
           src/imageviewer.h \
           src/panelgl.h \
           src/primitive.h \
           src/project_util.h \
           src/scene.h \
           src/select.h \
           src/settings.h \
           src/shader.h \
           src/sunshine.h \
           src/util.h \
           src/vertex_util.h \
           src/sunshineui.h \
           src/sunshine.h \
    src/attribute_editor.h \
    src/renderwidget.h \
    src/exceptions.h \
    src/python_bindings.h \
    src/object_tools.h \
    src/worktool.h \
    src/contextmenu.h \
    src/cursor_tools.h \
    src/render_util.h \
    src/material.h \
    src/light.h \
    src/transformable.h \
    src/bindable.h \
    src/menu_tools.h \
    src/vertex_tools.h
FORMS += src/sunshine.ui \
    src/renderwidget.ui \
    src/shader_tree_window.ui \
    src/node_widget.ui \
    src/shader_relationships_window.ui
SOURCES += src/camera.cpp \
           src/face_util.cpp \
           src/geometry.cpp \
           src/imageviewer.cpp \
           src/panelgl.cpp \
           src/primitive.cpp \
           src/project_util.cpp \
           src/scene.cpp \
           src/select.cpp \
           src/settings.cpp \
           src/shader.cpp \
           src/sunshine.cpp \
           src/util.cpp \
           src/vertex_util.cpp \
           src/sunshineui.cpp \
           src/main.cpp \
    src/attribute_editor.cpp \
    src/renderwidget.cpp \
    src/exceptions.cpp \
    src/python_bindings.cpp \
    src/object_tools.cpp \
    src/contextmenu.cpp \
    src/cursor_tools.cpp \
    src/draw_box_tool.cpp \
    src/render_util.cpp \
    src/material.cpp \
    src/light.cpp \
    src/transformable.cpp \
    src/bindable.cpp \
    src/cursor_tools/point_tool.cpp \
    src/cursor_tools/translate_tool.cpp \
    src/work_tools/split_polygon.cpp \
    src/vertex_tools.cpp
RESOURCES += src/plugins.qrc \
    src/icons.qrc \
    src/textures.qrc \
    src/glsl.qrc
