#include "register.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <PythonQt.h>

#include "sunshine.h"

CameraP Register::fetchCamera(QString name)
{
    QHashIterator<int,CameraP> cams = cameras();
    while (cams.hasNext()) {
        cams.next();
        cams.key();
        CameraP cam = cams.value();
        if (cam->name == name)
            return cam;
    }
    return CameraP();
}

QList<QString> Register::importExtensions()
{
    QList<QString> extensions;
    PythonQtObjectPtr MeshImporterClass = _context.getVariable("MeshImporter");
    foreach(QVariant ext, MeshImporterClass.call("extensions").toList()) {
        extensions << ext.toString();
    }
    return extensions;
}

void Register::importFile(QString fileName)
{
    PythonQtObjectPtr MeshImporterClass = _context.getVariable("MeshImporter");

    MeshImporterClass.call("processFile", QVariantList() << fileName);
}

Register::Register()
{
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

    PythonQt::self()->registerCPPClass("PrimitiveParts", "","", PythonQtCreateObject<PrimitivePartsWrapper>);
    //PythonQt::self()->registerCPPClass("MeshImporter", "", "", PythonQtCreateObject<VertexWrapper>);
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
    //PythonQt::self()->registerCPPClass("Vertex");
    //PythonQt::self()->registerClass(Vertex::metaObject());
}

MeshP Register::mesh(int key)
{
    return MeshP(_meshes[key]);
}

/*
void Register::clearScene()
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

CameraP Register::createCamera(QString name)
{
    std::cout << "Creating camera: " << name.toStdString() << std::endl;

    int key = uniqueCameraKey();
    QString unique = uniqueName(name);
    _cameras[key] = CameraP(new Camera(unique));
    _names += unique;
    return _cameras[key];
}

MeshP Register::createMesh(QString name)
{
    int key = uniqueMeshKey();
    QString unique = uniqueName(name);
    _meshes[key] = MeshP(new Mesh(key,unique));
    _names += unique;
    return _meshes[key];
}

int Register::uniqueCameraKey()
{
    int counter = 0;
    while (_cameras.contains(counter))
        counter++;
    return counter;
}

int Register::uniqueMeshKey()
{
    int counter = 0;
    while (_meshes.contains(counter))
        counter++;
    return counter;
}

QString Register::uniqueName(QString prefix)
{
    int counter = 1;
    QString name = prefix;
    while (_names.contains(name)) {
        name = QString("%s%d").arg(prefix).arg(counter);
        counter++;
    }
    _names += name;
    return name;
}
