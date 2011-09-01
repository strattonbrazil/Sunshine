#include "scene.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
//#include <PythonQt.h>

#include "sunshine.h"
#include "exceptions.h"

#include "python_bindings.h"

CameraP Scene::fetchCamera(QString name)
{
    QHashIterator<int,CameraP> cams = cameras();
    while (cams.hasNext()) {
        cams.next();
        cams.key();
        CameraP cam = cams.value();
        if (cam->name == name)
            return cam;
    }

    std::cerr << "Cannot find camera: " << name.toStdString() << std::endl;
    throw KeyErrorException();
    //return CameraP();
}


QList<QString> Scene::importExtensions()
{
    QList<QString> extensions;

    //object list = exec("MeshImporter.extensions()", _pyMainNamespace);

    try {
        object pyList = eval("MeshImporter.extensions()", _pyMainNamespace);
        std::vector<std::string> vec(len(pyList));
        for (std::size_t i = 0; i < vec.size(); ++i) {
          vec[i] = extract<std::string>(pyList[i]);
          extensions << QString(vec[i].c_str());
        }
    } catch (boost::python::error_already_set const &) {
        QString perror = parse_python_exception();
        std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
    }

    //std::vector<std::string> vec(len(pyList));
    /*
    for (std::size_t i = 0; i < vec.size(); ++i) {
      vec[i] = extract<std::string>(pyList[i]);
      std::cout << vec[i] << std::endl;
    }
    */

    return extensions;
}

void Scene::importFile(QString fileName)
{
    try {
        object processFileFunc = _pyMainModule.attr("MeshImporter").attr("processFile");
        processFileFunc(this, fileName);
    } catch (boost::python::error_already_set const &) {
        QString perror = parse_python_exception();
        std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
    }
}

Scene::Scene()
{
    Py_Initialize();
    _pyMainModule = import("__main__");
    _pyMainNamespace = _pyMainModule.attr("__dict__");

    createPythonBindings();

    evalPythonFile(":/plugins/meshImporter.py");
    evalPythonFile(":/plugins/objImporter.py");

    /*
    // Create a new lua state
    lua_State *myLuaState = lua_open();
    //luaopen_io(myLuaState);


    // Connect LuaBind to this lua state
    luabind::open(myLuaState);
    luaL_openlibs(myLuaState);

    // Define a lua function that we can call
    luaL_dostring(myLuaState,
                  "function add(first, second)\n"
                  "  return first + second\n"
                  "end\n"
                    );

    std::cout << "Result: " << luabind::call_function<int>(myLuaState, "add", 2, 3) << std::endl;

//luaL_dostring(myLuaState, "_meshImporters = {}");

    // read
    luaL_dostring(myLuaState, "print('test')");
    loadLuaFile(myLuaState, ":/plugins/mesh_importer.lua");
    loadLuaFile(myLuaState, ":/plugins/obj_importer.lua");


    //std::cout << lines.toStdString() << std::endl;

    //luaL_dostring(myLuaState, "_meshImporters[]")
*/
    /*

    PythonQt::init(PythonQt::RedirectStdOut);
    PythonQt::self()->setImporter(NULL);
    QObject::connect(PythonQt::self(), SIGNAL(pythonStdOut(QString)), this, SLOT(pythonStdOut(QString)));
    QObject::connect(PythonQt::self(), SIGNAL(pythonStdErr(QString)), this, SLOT(pythonStdOut(QString)));

    _context = PythonQt::self()->getMainModule();

    // add a QObject as variable of name "example" to the namespace of the __main__ module
    //PyExampleObject example;
    //context.addObject("example", &example);

    // do something
    //context.evalScript("print example");
    //context.evalScript("def multiply(a,b):\n  return a*b;\n");
    //QVariantList args;
    //args << 42 << 47;
    //QVariant result = context.call("multiply", args);

    //std::cout << result.toString().toStdString() << std::endl;

    PythonQt::self()->addWrapperFactory(new CustomFactory());
    PythonQt::self()->SceneCPPClass("PrimitiveParts");
    PythonQt::self()->addClassDecorators(new PrimitivePartsConstructor());
    //PythonQt::self()->SceneCPPClass("Vertex", "", "", PythonQtCreateObject<VertexWrapper>);
    //context.evalScript("print('here');");

    //context.
    //PythonQt::self()->createModuleFromFile("meshImporter", ":/plugins/meshImporter.py");
//    context.evalFile(":/plugins/meshImporter.py");
    _context.evalScript(QString("import sys\n"));
    _context.evalScript("sys.path.append(':')\n");
    _context.evalScript("sys.path.append(':/plugins')\n");
    _context.evalFile(":/plugins/objImporter.py");

    //QFile file(":/plugins/textfinder.ui");
      //   file.open(QFile::ReadOnly);
*/
    /*
    QString pluginPath(getenv("SUNSHINE_PLUGIN_PATH"));
    std::cout << "Plugin path: " << pluginPath.toStdString() << std::endl;

    QDir scriptDir(pluginPath);
    std::cout << scriptDir.path().toStdString() << std::endl;
    foreach (QFileInfo fileInfo, scriptDir.entryInfoList()) {
        std::cout << fileInfo.absoluteFilePath().toStdString() << std::endl;
    }
    */

    //PythonQt::self()->addDecorators(new VertexDecorator());
    //PythonQt::self()->SceneCPPClass("Vertex");
    //PythonQt::self()->SceneClass(Vertex::metaObject());
}

MeshP Scene::mesh(int key)
{
    return MeshP(_meshes[key]);
}

/*
void Scene::clearScene()
{
    _meshes.clear();
    _cameras.clear();
    _names.clear();

    QCoreApplication::addLibraryPath("/home/stratton/sunshine/");

    // output library path
    foreach (QString path, QCoreApplication::libraryPaths()) {
        std::cout << path.toStdString() << std::endl;
    }

    // import any plugins
    instance->_engine = QScriptEngineP(new QScriptEngine());
    QScriptValue sc = instance->_engine->importExtension("core.importer.obj");
    if (instance->_engine->hasUncaughtException()) {
        sc = instance->_engine->uncaughtException();
        int line = instance->_engine->uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line: " << line << " (" << sc.toString().toStdString() << ")" << std::endl;
        //return;
    }

    sc = instance->_engine->evaluate("core.importer.obj.extension;");
    if (instance->_engine->hasUncaughtException()) {
        int line = instance->_engine->uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line" << line << ":" << sc.toString().toStdString() << std::endl;
    }
    std::cerr << sc.toString().toStdString() << std::endl;
}
*/

CameraP Scene::createCamera(QString name)
{
    std::cout << "Creating camera: " << name.toStdString() << std::endl;

    int key = uniqueCameraKey();
    QString unique = uniqueName(name);
    _cameras[key] = CameraP(new Camera(unique));
    _names += unique;
    return _cameras[key];
}

MeshP Scene::createMesh(QString name)
{
    int key = uniqueMeshKey();
    QString unique = uniqueName(name);
    _meshes[key] = MeshP(new Mesh(SceneP(this),key,unique));
    _names += unique;
    return _meshes[key];
}

int Scene::uniqueCameraKey()
{
    int counter = 0;
    while (_cameras.contains(counter))
        counter++;
    return counter;
}

int Scene::uniqueMeshKey()
{
    int counter = 0;
    while (_meshes.contains(counter))
        counter++;
    return counter;
}

QString Scene::uniqueName(QString prefix)
{
    int counter = 1;
    QString name = prefix;
    while (_names.contains(name)) {
        name = QString("%1%2").arg(prefix).arg(counter);
        counter++;
    }
    _names += name;
    return name;
}
