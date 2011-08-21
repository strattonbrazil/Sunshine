from meshImporter import *
from PythonQt import *

PrimitiveParts()

class ObjImporter(MeshImporter):
    def extension(self):
        return '.obj'
    def importFile(self, fileName):
        print('Importing mesh: %s' % fileName)
        def newGroup(name='default'):
            return {'name' : name,
                    'vertices' : [],
                    'normals' : [],
                    'faces' : []}
        groups = []



        f = open(fileName, 'r')
        for line in f.readlines():
            tokens = line.split()
            if len(tokens) == 0:
                continue
            print(tokens)

            if tokens[0] == 'g':
                groups.append(newGroup(tokens[1]))
            elif tokens[0] == 'v':
                groups[-1]['vertices'].append([tokens[1],tokens[2],tokens[3]])
            elif tokens[0] == 'vn':
                groups[-1]['normals'].append([tokens[1],tokens[2],tokens[3]])
            elif tokens[0] == 'f':
                groups[-1]['faces'].append(map(lambda token: int(token.split('/')[0])-1, tokens[1:]))
        f.close()

        for group in groups:
            parts = PrimitiveParts()

        print(groups)

MeshImporter.register(ObjImporter())
