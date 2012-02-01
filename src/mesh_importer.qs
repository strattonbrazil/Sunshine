var meshImporters = new Array();
meshImporters['ackbar'] = 3;
meshImporters['ackfoo'] = 6;

Object.prototype.keys = function()
{
  var keys = [];
  for(var i in this) if (this.hasOwnProperty(i))
  {
    keys.push(i);
  }
  return keys;
}

var registerImporter = function(extension, f) {
    meshImporters[extension] = f;
}

var extensions = function() {
    return meshImporters.keys();
}

var processFile = function(scene, file) {
    var extension = file.split('.').pop();
    var importFunc = meshImporters[extension];
    print("importing file: " + file);
    print("extension: " + extension);
    //print("func: " + importFunc);
    importFunc(scene, file);
}

