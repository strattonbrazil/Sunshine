var materialTypes = new Array();

var registerMaterial = function(typeName, material) {
    materialTypes[typeName] = material;
}

// loads an image for sampling
//
var fileLoader = new Object();
fileLoader.name = "File";
fileLoader.inputs = [];

registerMaterial(fileLoader.name, fileLoader);

/*
* material provides (list of channels that can be overriden
*   -list of inputs that can be overriden (diffuse, bump, reflectivity)
*/



