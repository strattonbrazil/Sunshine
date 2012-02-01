#include "python_bindings.h"
#include "exceptions.h"



#include <primitive.h>

#include "scene.h"
#include "primitive.h"

/*
ExampleDecorator::ExampleDecorator()
{
    std::cout << "right here" << std::endl;
}
*/

void createPythonBindings()
{
    /*
    boost::python::to_python_converter<QString,QStringToPythonStr>();

    PyImport_AppendInittab("primitive", &initprimitive);
    PyImport_AppendInittab("util", &initutil);
    PyImport_AppendInittab("geometry", &initgeometry);

    boost::python::register_ptr_to_python< boost::shared_ptr<Scene> >();
    PyImport_AppendInittab("scene", &initscene);

    IntQListFromPythonList();
    IntQListVectorFromPythonList();
    QPoint3VectorFromPythonList();
*/



    // try PythonQt bindings
    //PythonQt::self()->registerClass(&Mesh::staticMetaObject, "geometry");
    //PythonQt::priv()->registerClass("Scene", "", "core", PythonQtCreateObject<PythonQtWrapper_Scene>);
    //PythonQt::priv()->registerClass(&Scene::staticMetaObject, "core", PythonQtCreateObject<PythonQtWrapper_Scene>);

    //PythonQt::self()->addDecorators(new ExampleDecorator());
    //PythonQt::self()->registerCPPClass("PrimitiveParts");//, "", "primitives"PythonQtCreateObject<PythonQtWrapper_QAccessibleBridge>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_QAccessibleBridge>, module, 0);

    //PythonQt::self()->registerClass(&PrimitiveParts::staticMetaObject, "primitives");


    //PythonQt::registerClass(Mesh::metaObject());

    //QSceneFromPythonScene();
}



