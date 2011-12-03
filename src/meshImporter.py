class MeshImporter:
    _importers = {}
    @staticmethod
    def register(importer):
        MeshImporter._importers[importer.extension()] = importer
    @staticmethod
    def extensions():
        extensions = MeshImporter._importers.keys()
        return extensions
    def extension(self):
        raise NotImplementedError()
    @staticmethod
    def processFile(scene, fileName):
        extension = '.' + fileName.split('.')[-1]
        importer = MeshImporter._importers[extension]
        importer.importFile(scene, fileName)
    def importFile(self, fileName):
        raise NotImplementedError()
