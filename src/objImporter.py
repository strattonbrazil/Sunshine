#from meshImporter import *
from primitive import PrimitiveParts
from util import Point3,Vector3
from geometry import Mesh
from scene import Scene

class ObjImporter(MeshImporter):
    def extension(self):
        return '.obj'
    def importFile(self, scene, fileName):
        print('Importing mesh: %s' % fileName)
        def newGroup(name='default'):
            return {'name' : name,
                    'vertices' : [],
                    'normals' : [],
                    'faces' : []}

        group = newGroup()

        f = open(fileName, 'r')
        for line in f.readlines():
            tokens = line.split()
            if len(tokens) == 0:
                continue

            if tokens[0] == 'g':
                pass
#                if len(tokens) > 1:
#                    groups.append(newGroup(tokens[1]))
#                else:
#                    groups.append(newGroup('default'))
            elif tokens[0] == 'v':
                group['vertices'].append(Point3(float(tokens[1]),float(tokens[2]),float(tokens[3])))
            elif tokens[0] == 'vn':
                group['normals'].append(Vector3(float(tokens[1]),float(tokens[2]),float(tokens[3])))
            elif tokens[0] == 'f':
                group['faces'].append(map(lambda token: int(token.split('/')[0])-1, tokens[1:]))
        f.close()

        parts = PrimitiveParts()
        parts.points = group['vertices']
        parts.faces = group['faces']

        Mesh.buildByIndex(scene, parts)

MeshImporter.register(ObjImporter())
