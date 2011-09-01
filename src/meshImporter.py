print('Importing MeshImporter')

class MeshImporter:
    _importers = {}
    @staticmethod
    def register(importer):
        MeshImporter._importers[importer.extension()] = importer
    @staticmethod
    def extensions():
        extensions = MeshImporter._importers.keys()
        print(extensions)
        return extensions
    def extension(self):
        raise NotImplementedError()
    @staticmethod
    def processFile(scene, fileName):
        print('Importing file: %s' % fileName)
        extension = '.' + fileName.split('.')[-1]
        importer = MeshImporter._importers[extension]
        print(importer)
        importer.importFile(scene, fileName)
    def importFile(self, fileName):
        raise NotImplementedError()
