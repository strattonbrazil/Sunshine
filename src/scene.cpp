#include "scene.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <QStandardItem>

#include "sunshine.h"
#include "exceptions.h"

#include "python_bindings.h"
#include "object_tools.h"
#include "vertex_tools.h"

#include <QScriptEngine>
#include <QProcess>
#include <assimp/assimp.hpp>
#include <assimp/aiPostProcess.h>
#include <assimp/aiScene.h>

QList<QString> Scene::assetsByType(int assetType)
{
    QList<QString> meshAssets;
    foreach(QString assetName, _assets.keys()) {
        if (_assets[assetName]->assetType() == assetType)
            meshAssets << assetName;
    }
    return meshAssets;
}

QList<QString> Scene::meshes()
{
    return assetsByType(AssetType::MESH_ASSET);
}

QList<QString> Scene::lights()
{
    return assetsByType(AssetType::LIGHT_ASSET);
}

QList<QString> Scene::materials()
{
    return assetsByType(AssetType::MATERIAL_ASSET);
}

QList<QString> Scene::cameras()
{
    return assetsByType(AssetType::CAMERA_ASSET);
}
//#include <assimp/aiScene.h>
//#include <assimp/aiPostProcess.h>

QList<QString> Scene::importExtensions()
{
    Assimp::Importer importer;
    std::string fExtensions;
    importer.GetExtensionList(fExtensions);

    QString extensions = QString::fromStdString(fExtensions);
    QList<QString> extensionList;
    foreach(QString extension, extensions.split(";")) {
        extensionList.append(extension.split("*.")[1]);
    }

    //extensions.append("obj");
    return extensionList;
}

bool loadFile(QString fileName, QScriptEngine *engine)
{
    // avoid loading files more than once
    static QSet<QString> loadedFiles;
    QFileInfo fileInfo(fileName);
    QString absoluteFileName = fileInfo.absoluteFilePath();
    QString absolutePath = fileInfo.absolutePath();
    QString canonicalFileName = fileInfo.canonicalFilePath();
    if (loadedFiles.contains(canonicalFileName)) {
        return true;
    }
    loadedFiles.insert(canonicalFileName);
    QString path = fileInfo.path();

    // load the file
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString contents = stream.readAll();
        file.close();

        int endlineIndex = contents.indexOf('\n');
        QString line = contents.left(endlineIndex);
        int lineNumber = 1;

        // strip off #!/usr/bin/env qscript line
        if (line.startsWith("#!")) {
            contents.remove(0, endlineIndex+1);
            ++lineNumber;
        }

        // set qt.script.absoluteFilePath
        QScriptValue script = engine->globalObject().property("qs").property("script");
        QScriptValue oldFilePathValue = script.property("absoluteFilePath");
        QScriptValue oldPathValue = script.property("absolutePath");
        script.setProperty("absoluteFilePath", engine->toScriptValue(absoluteFileName));
        script.setProperty("absolutePath", engine->toScriptValue(absolutePath));

        QScriptValue r = engine->evaluate(contents, fileName, lineNumber);
        if (engine->hasUncaughtException()) {
            QStringList backtrace = engine->uncaughtExceptionBacktrace();
            qDebug() << QString("    %1\n%2\n\n").arg(r.toString()).arg(backtrace.join("\n"));
            return true;
        }
        script.setProperty("absoluteFilePath", oldFilePathValue); // if we come from includeScript(), or whereever
        script.setProperty("absolutePath", oldPathValue); // if we come from includeScript(), or whereever
    } else {
        return false;
    }
    return true;
}

QScriptValue includeScript(QScriptContext *context, QScriptEngine *engine)
{
    QString currentFileName = engine->globalObject().property("qs").property("script").property("absoluteFilePath").toString();
    QFileInfo currentFileInfo(currentFileName);
    QString path = currentFileInfo.path();
    QString importFile = context->argument(0).toString();
    QFileInfo importInfo(importFile);
    if (importInfo.isRelative()) {
        importFile =  path + "/" + importInfo.filePath();
    }
    if (!loadFile(importFile, engine)) {
        return context->throwError(QString("Failed to resolve include: %1").arg(importFile));
    }
    return engine->toScriptValue(true);
}

QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}

void Scene::importFile(QString fileName)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( fileName.toStdString(),

                                              aiProcess_FindDegenerates |
            aiProcess_JoinIdenticalVertices);

    // If the import failed, report it
    if(!scene)
    {
        std::cerr << "Unable to import file: " << fileName << std::endl;
    }
    else {
        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            PrimitiveParts parts;

            // add vertices
            for (int j = 0; j < mesh->mNumVertices; j++) {
                aiVector3D vertex = mesh->mVertices[j];
                Point3 v(vertex.x, vertex.y, vertex.z);
                parts.points.append(v);
            }

            // add faces
            for (int j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[j];
                QList<int> indices;
                for (int k = 0; k < face.mNumIndices; k++)
                    indices.append(face.mIndices[k]);
                parts.faces.append(indices);
            }

            Mesh* m = Mesh::buildByIndex(parts);
            this->addAsset("mesh", m);
        }
    }
}

QStringList Scene::materialTypes()
{
    QStringList typeNames;
    QScriptValue typeNamesVal = _engine.evaluate("materialTypes.keys();");

    const int numTypes = typeNamesVal.property("length").toInteger();

    for (int i = 0; i < numTypes; i++) {
        QString matType = qScriptValueToValue<QString>(typeNamesVal.property(i));
        typeNames.append(matType);
    }


    return typeNames;
}

Material* Scene::buildMaterial(QString matType) {
    QScriptValue materialDef = _engine.evaluate("materialTypes[\"" + matType + "\"];");

    QStringList inputs;
    QScriptValue materialInputs = materialDef.property("inputs");
    const int numInputs = materialInputs.property("length").toInteger();
    for (int i = 0; i < numInputs; i++) {
        inputs << materialInputs.property(i).toString();
    }

    return new ScriptMaterial(inputs);
}

#if 0
QScriptValue Scene::processFile(QScriptEngine &engine, QString filePath)
{
    //QString meshImporterPath = ":/qs/mesh_importer.qs";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return QScriptValue();
    }
    QTextStream stream(&file);
    QString contents = stream.readAll();
    file.close();
    //std::cout << contents << std::endl;
    QScriptValue result = engine.evaluate(contents, filePath);
    if (engine.hasUncaughtException()) {
        int line = engine.uncaughtExceptionLineNumber();
        qDebug() << "uncaught exception at line " << line << ":" << result.toString();
    }
    return result;
}
#endif

bool Scene::hasMeshSelected()
{
    foreach(QString meshName, meshes()) {
        Mesh* mesh = this->mesh(meshName);
        if (mesh->isSelected())
            return true;
    }
    return false;
}

bool Scene::hasVertexSelected()
{
    foreach(QString meshName, meshes()) {
        Mesh* mesh = this->mesh(meshName);
        if (mesh->isSelected()) {
            QHashIterator<int,Vertex*> vertices = mesh->vertices();
            while (vertices.hasNext()) {
                vertices.next();
                Vertex* vertex = vertices.value();
                if (vertex->isSelected())
                    return true;
            }
        }
    }
    return false;
}

Scene::Scene()
{
    //Py_Initialize();

    //_pyMainModule = import("__main__");
    //_pyMainNamespace = _pyMainModule.attr("__dict__");

    createPythonBindings();

    //initScriptEngine();

    //evalPythonFile(":/plugins/meshImporter.py");
    //evalPythonFile(":/plugins/objImporter.py");

    // do something
    //pyContext.evalScript("def multiply(a,b):\n  return a*b;\n");
    //QVariantList args;
    //args << 42 << 47;
    //QVariant result = pyContext.call("multiply", args);
    //std::cout << result.toString().toStdString() << std::endl;

    _tools << new TranslateTransformable();
    _tools << new ScaleTransformable();
    _tools << new SplitPolygon();
    _tools << new VertexNormalizer();
}

#if 0
void Scene::initScriptEngine()
{
    QScriptValue global = _engine.globalObject();
    // add the qt object
    global.setProperty("qs", _engine.newObject());
    // add a 'script' object
    QScriptValue script = _engine.newObject();
    global.property("qs").setProperty("script", script);
    // add a 'system' object
    QScriptValue system = _engine.newObject();
    global.property("qs").setProperty("system", system);

    // add os information to qt.system.os
#ifdef Q_OS_WIN32
    QScriptValue osName = _engine.toScriptValue(QString("windows"));
#elif defined(Q_OS_LINUX)
    QScriptValue osName = _engine.toScriptValue(QString("linux"));
#elif defined(Q_OS_MAC)
    QScriptValue osName = _engine.toScriptValue(QString("mac"));
#elif defined(Q_OS_UNIX)
    QScriptValue osName = _engine.toScriptValue(QString("unix"));
#endif
    system.setProperty("os", osName);

    // add environment variables to qt.system.env
    QMap<QString,QVariant> envMap;
    QStringList envList = QProcess::systemEnvironment();
    foreach (const QString &entry, envList) {
        QStringList keyVal = entry.split('=');
        if (keyVal.size() == 1)
            envMap.insert(keyVal.at(0), QString());
        else
            envMap.insert(keyVal.at(0), keyVal.at(1));
    }
    system.setProperty("env", _engine.toScriptValue(envMap));

    // add the include functionality to qt.script.include
    script.setProperty("include", _engine.newFunction(includeScript));
    // add the importExtension functionality to qt.script.importExtension
    script.setProperty("importExtension", _engine.newFunction(importExtension));

    QScriptValue ppProto = _engine.newObject();
    ppProto.setProperty("setVertices", _engine.newFunction(PrimitiveParts_setVertices));
    ppProto.setProperty("setFaces", _engine.newFunction(PrimitiveParts_setFaces));
    QScriptValue primitivePartsCtor = _engine.newFunction(constructPrimitiveParts, ppProto);
    _engine.globalObject().setProperty("PrimitiveParts", primitivePartsCtor);


    //QScriptValue meshProto = _engine.newObject();
    //ppProto.setProperty("setVertices", _engine.newFunction(PrimitiveParts_setVertices));
    //ppProto.setProperty("setFaces", _engine.newFunction(PrimitiveParts_setFaces));
    //QScriptValue meshCtor = _engine.newFunction(constructMesh, meshProto);
    //_engine.globalObject().setProperty("Mesh", meshCtor);

    // TODO: attach this to the Mesh QtScript prototype
    //
    QScriptValue meshFactoryFunc = _engine.newFunction(Mesh_buildByIndex);
    _engine.globalObject().setProperty("buildByIndex", meshFactoryFunc);


    //_engine.globalObject().setProperty("PrimitiveParts", _engine.newFunction(PrimitiveParts_ctor));
    /*
    QScriptValue ppObject = _engine.newObject();
    ppObject.globalObject().setProperty("QPoint", _engine.newFunction(QPoint_ctor));
    ppObject.setProperty("PrimitiveParts", _engine.newFunction(getSetPrimitiveParts), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
    //ppObject.setProperty("PrimitiveParts", _engine.newFunction(setFoo), QScriptValue::PropertySetter);
    */

    //_engine.importExtension("qt.core");
    // setup model importing
    //
    processFile(_engine, ":/qs/mesh_importer.qs");
    processFile(_engine, ":/qs/obj_importer.qs");

    processFile(_engine, ":/qs/material_interface.qs");
}
#endif

QString Scene::addAsset(QString name, Bindable* asset)
{
    QString unique = uniqueName(name);
    _assets[unique] = asset;

    if (asset->assetType() == AssetType::MESH_ASSET) {
        QIcon icon = QIcon(":/icons/mesh_icon.png");
        _hierarchyModel.appendRow(new QStandardItem(icon, unique));
    }
    else if (asset->assetType() == AssetType::MATERIAL_ASSET) {
        QIcon icon = QIcon(":/icons/material_icon.png");
        _shaderTreeModel.addMaterial(unique, qobject_cast<Material*>(asset));
        //_shaderTreeModel.appendRow(new QStandardItem(icon, unique));
    }
    else if (asset->assetType() == AssetType::CAMERA_ASSET) {
        QIcon icon = QIcon(":/icons/camera_icon.png");
        _hierarchyModel.appendRow(new QStandardItem(icon, unique));
    }
    else if (asset->assetType() == AssetType::LIGHT_ASSET) {
        QIcon icon = QIcon(":/icons/light_icon.png");
        _hierarchyModel.appendRow(new QStandardItem(icon, unique));
    }



    SunshineUi::selectAsset(unique);

    return unique;
}

bool Scene::setData(const QModelIndex &index, const QVariant &value, int role)
{
    /*
    QString oldName = index.data(role).toString();
    QString newName = value.toString();
    if (_names.contains(newName)) // must maintain unique names
        return false;

    _names.remove(oldName);
    _names.insert(newName);
    */
    return true;

    //return QStandardItemModel::setData(index, value, role);
}



QString Scene::uniqueName(QString prefix)
{
    int counter = 1;
    QString name = prefix;
    while (_assets.contains(name)) {
        name = QString("%1%2").arg(prefix).arg(counter);
        counter++;
    }

    return name;
}

