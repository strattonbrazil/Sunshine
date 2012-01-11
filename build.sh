#!/bin/sh

cd src
qmake-qt4
make
cd ..

cd hybridpy

QTGUI_INC=/usr/include/QtGui
QTCORE_INC=/usr/include/QtCore
QTTYPESYSTEM=/usr/share/PySide/typesystems

generatorrunner --generatorSet=shiboken \
    ../data/global.h \
    --include-paths=../src:$QTCORE_INC:$QTGUI_INC:/usr/include \
    --typesystem-paths=../data:$QTTYPESYSTEM \
    --output-directory=. \
    ../data/typesystem.xml



qmake-qt4
make
cd ..

rm -rf Sunshine.so
ln -s libSunshine.so Sunshine.so
