#include "python_bindings.h"
#include "exceptions.h"

#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

using namespace boost::python;

#include "primitive.h"
BOOST_PYTHON_MODULE(primitive)
{
    class_<PrimitiveParts>("PrimitiveParts")
            .def_readwrite("points", &PrimitiveParts::points)
            .def_readwrite("faces", &PrimitiveParts::faces);
}

#include "geometry.h"
BOOST_PYTHON_MODULE(geometry)
{
    class_<Mesh>("Mesh")
            .def(init<SceneP,int,QString>())
            .def("buildByIndex", &Mesh::buildByIndex)
            .staticmethod("buildByIndex");

}

BOOST_PYTHON_MODULE(util)
{
    class_<Point3>("Point3")
            .def(init<float, float, float>());
    class_<Vector3>("Vector3")
            .def(init<float,float,float>());

}

#include "scene.h"
namespace boost { namespace python {
    template <typename T>
    struct pointee< QSharedPointer<T> > {
        typedef T type;
    };
}}

// in some namespace where ADL will find it...
//template <typename T*>
inline Scene* get_pointer(QSharedPointer<Scene> const &p) {
   return p.data(); // or whatever
}

BOOST_PYTHON_MODULE(scene)
{
    class_<Scene, QSharedPointer<Scene> >("Scene");
}
/*
struct QSceneFromPythonScene
{
    static PyObject* convert(Scene const& s)
    {
        return boost::python::incref(boost::python::object(SceneP(&s)));
    }
};
*/

struct QStringToPythonStr
{
    static PyObject* convert(QString const& s)
    {
        return boost::python::incref(boost::python::object(s.toLatin1().constData()).ptr());
    }
};

struct QStringFromPythonStr
{
    static void* convertible(PyObject* obj_ptr) {
        if (!PyString_Check(obj_ptr))
            return 0;
        return obj_ptr;
    }
};

struct IntQListFromPythonList
{
    IntQListFromPythonList()
    {
        boost::python::converter::registry::push_back(&convertible,
                                                      &construct,
                                                      boost::python::type_id<QList<int> >());
    }

    // Determine if obj_ptr can be converted in a <QList<int>
    static void* convertible(PyObject* obj_ptr)
    {
        if (!PyList_Check(obj_ptr))
            return 0;
        boost::python::list l( handle<>(borrowed(obj_ptr)) );

        int n = PyList_Size(obj_ptr);
        for (int i = 0; i < n; ++i) {
            if (!boost::python::extract<int>(l[i]).check()) {
                return 0;
            }
        }

        return obj_ptr;
    }



    // Convert obj_ptr into a QList<int>
    static void construct(PyObject* obj_ptr,
                          boost::python::converter::rvalue_from_python_stage1_data* data) {
        // Extract the character data from the python string
        boost::python::list l( handle<>(borrowed(obj_ptr)) );

        // grab pointer to memory into which to construct the new QVector
        void* storage = ((boost::python::converter::rvalue_from_python_storage<QList<int> >*)data)->storage.bytes;
        QList<int>* list = new (storage)QList<int>();

        int n = PyList_Size(obj_ptr);
        for (int i = 0; i < n; ++i) {
            list->append(boost::python::extract<int>(l[i]));
        }

        // stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

struct IntQListVectorFromPythonList
{
    IntQListVectorFromPythonList()
    {
        boost::python::converter::registry::push_back(&convertible,
                                                      &construct,
                                                      boost::python::type_id<QVector<QList<int> > >());
    }

    // Determine if obj_ptr can be converted in a QVector<QList<int>>
    static void* convertible(PyObject* obj_ptr)
    {
        std::cout << "can convert to IntQListVector?" << std::endl;
        try {
            if (!PyList_Check(obj_ptr))
                return 0;
            boost::python::list l( handle<>(borrowed(obj_ptr)) );

            int n = PyList_Size(obj_ptr);
            for (int i = 0; i < n; ++i) {
                std::cout << boost::python::extract<QList<int> >(l[i]).check() << std::endl;
                if (!boost::python::extract<QList<int> >(l[i]).check()) {
                    std::cout << "erp" << std::endl;
                    return 0;
                }
            }

            return obj_ptr;
        } catch (boost::python::error_already_set const &) {
            QString perror = parse_python_exception();
            std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
            return 0;
        }
        return obj_ptr;
    }

    // Convert obj_ptr into a QVector<Point3>
    static void construct(PyObject* obj_ptr,
                          boost::python::converter::rvalue_from_python_stage1_data* data) {

        // Extract the character data from the python string
        boost::python::list l( handle<>(borrowed(obj_ptr)) );

        // grab pointer to memory into which to construct the new QVector
        void* storage = ((boost::python::converter::rvalue_from_python_storage<QString>*)data)->storage.bytes;
        QVector<QList<int> >* list = new (storage)QVector<QList<int> >();

        int n = PyList_Size(obj_ptr);
        for (int i = 0; i < n; ++i) {
            list->append(boost::python::extract<QList<int> >(l[i]));
        }

        // stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

struct QPoint3VectorFromPythonList
{
    QPoint3VectorFromPythonList()
    {
        boost::python::converter::registry::push_back(&convertible,
                                                      &construct,
                                                      boost::python::type_id<QVector<Point3> >());
    }
    // Determine if obj_ptr can be converted in a QVector<Point3>
    static void* convertible(PyObject* obj_ptr)
    {

        try {
            if (!PyList_Check(obj_ptr))
                return 0;
            boost::python::list l( handle<>(borrowed(obj_ptr)) );

            int n = PyList_Size(obj_ptr);
            for (int i = 0; i < n; ++i) {
                if (!boost::python::extract<Point3>(l[i]).check()) {
                    return 0;
                }
            }

            return obj_ptr;
        } catch (boost::python::error_already_set const &) {
            QString perror = parse_python_exception();
            std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
            return 0;
        }
        return obj_ptr;
    }

    // Convert obj_ptr into a QVector<Point3>
    static void construct(PyObject* obj_ptr,
                          boost::python::converter::rvalue_from_python_stage1_data* data) {
        // Extract the character data from the python string
        boost::python::list l( handle<>(borrowed(obj_ptr)) );

        // grab pointer to memory into which to construct the new QVector
        void* storage = ((boost::python::converter::rvalue_from_python_storage<QVector<Point3> >*)data)->storage.bytes;
        QVector<Point3>* list = new (storage)QVector<Point3>();

        int n = PyList_Size(obj_ptr);
        for (int i = 0; i < n; ++i) {
            list->append(boost::python::extract<Point3>(l[i]));
        }

        // stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

void createPythonBindings()
{
    std::cout << "creating bindings" << std::endl;
    boost::python::to_python_converter<QString,QStringToPythonStr>();

    PyImport_AppendInittab("primitive", &initprimitive);
    PyImport_AppendInittab("util", &initutil);
    PyImport_AppendInittab("geometry", &initgeometry);
    PyImport_AppendInittab("scene", &initscene);

    IntQListFromPythonList();
    IntQListVectorFromPythonList();
    QPoint3VectorFromPythonList();
    //QSceneFromPythonScene();
}



