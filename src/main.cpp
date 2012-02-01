#include <QtGui/QApplication>

#include "sunshine.h"

//#include <iostream>
//using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setLibraryPaths(a.libraryPaths() << "/home/stratton/sunshine/qtscript_plugins/");


    Sunshine w;
    w.show();
    return a.exec();
}
