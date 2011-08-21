print('ack')

obj_importer = MeshImporter()
obj_importer.extension = function() return '.ext' end
obj_importer.processFile = function(fileName)
   print('opening ' + fileName)
end

print(obj_importer.extension())
obj_importer.processFile("test.obj")

print('done')
