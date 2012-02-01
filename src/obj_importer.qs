//Importer.loadQtBinding("qt.core");
qs.script.importExtension("qt.core");
qs.script.importExtension("qt.gui");

var objImportFunc = function(scene, fileName) {
    var newGroup = function(name) {
        var g = new Array();
        g["name"] = name;
        g["vertices"] = [];
        g["normals"] = [];
        g["faces"] = [];
        return g;
    }

    var cornerToVertices = function(faceToken) {
        return faceToken.split('/')[0]-1;
    }

    group = newGroup();

    print("importing obj: " + fileName);
    var file = new QFile(fileName);
    if (!file.open(new QIODevice.OpenMode(QIODevice.ReadWrite))) {
        print("Unable to open file: " + fileName);
        return;
    }
    var stream = new QTextStream(file);
    var lines = stream.readAll().split("\n");
    for (var i = 0; i < lines.length; i++) {
        line = lines[i];

        var tokens = line.replace(/^\s+|\s+$/g, "").replace(/\s+/g, " ").split(" ");

        if (tokens.length == 0) {
            continue;
        }
        if (tokens[0] == "#") {
            continue;
        }



        if (tokens[0] == "g")
            var x = 5;
        else if (tokens[0] == 'v')
            group["vertices"].push(new QVector3D(tokens[1],tokens[2],tokens[3]));
        else if (tokens[0] == 'vn')
            group["normals"].push(new QVector3D(tokens[1],tokens[2],tokens[3]));
        else if (tokens[0] == 'f')
            group["faces"].push(tokens.slice(1).map(cornerToVertices)); //map(lambda token: int(token.split('/')[0])-1, tokens[1:]))
        //print(line);



    }

    print("# of vertices: " + group["vertices"].length);
    print("# of faces: " + group["faces"].length);

    pp = new PrimitiveParts();
    pp.setVertices(group["vertices"]);
    pp.setFaces(group["faces"]);
    //print(pp);

    mesh = buildByIndex(pp);
    scene.addMesh("mesh", mesh);
}

registerImporter("obj", objImportFunc);
