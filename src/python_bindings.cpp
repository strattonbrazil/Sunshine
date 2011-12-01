#include "python_bindings.h"
#include "exceptions.h"

#include <PythonQt.h>


#include <primitive.h>

#include "scene.h"
#include "primitive.h"

/*
ExampleDecorator::ExampleDecorator()
{
    std::cout << "right here" << std::endl;
}
*/

#include "com_trolltech_qt_gui10.h"



void createPythonBindings()
{
    std::cout << "creating bindings" << std::endl;
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
    PythonQt::self()->registerClass(&Scene::staticMetaObject);

    //PythonQt::self()->addDecorators(new ExampleDecorator());
    //PythonQt::self()->registerCPPClass("PrimitiveParts");//, "", "primitives"PythonQtCreateObject<PythonQtWrapper_QAccessibleBridge>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_QAccessibleBridge>, module, 0);

    //PythonQt::self()->registerClass(&PrimitiveParts::staticMetaObject, "primitives");
    PythonQt::priv()->registerCPPClass("PrimitiveParts", "", "primitives", PythonQtCreateObject<PythonQtWrapper_PrimitiveParts>);
    PythonQt::priv()->registerCPPClass("Mesh", "", "geometry", PythonQtCreateObject<PythonQtWrapper_Mesh>);
    PythonQt::priv()->registerCPPClass("QVector3D", "", "QtGui", PythonQtCreateObject<PythonQtWrapper_QVector3D>, 0, 0, PythonQt::Type_Divide|PythonQt::Type_InplaceDivide|PythonQt::Type_NonZero|PythonQt::Type_InplaceSubtract|PythonQt::Type_InplaceAdd|PythonQt::Type_RichCompare|PythonQt::Type_Subtract|PythonQt::Type_InplaceMultiply|PythonQt::Type_Multiply|PythonQt::Type_Add);
    //PythonQt::self()->registerCPPClass("QVector3D", "", "QtGui", PythonQtCreateObject<PythonQtWrapper_QVector3D>);
    //PythonQtRegisterToolClassesTemplateConverter(QVector3D);

    //PythonQt::registerClass(Mesh::metaObject());

    //QSceneFromPythonScene();
}



