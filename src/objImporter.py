from PythonQt import *
#from meshImporter import *

from PythonQt.primitives import PrimitiveParts
from PythonQt.geometry import Mesh
#from util import Point3,Vector3
#from geometry import Mesh
#from scene import Scene


from PythonQt.QtGui import QVector3D


class ObjImporter(MeshImporter):
    def extension(self):
        return '.obj'
    def importFile(self, scene, fileName):
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
                group['vertices'].append(QVector3D(float(tokens[1]),float(tokens[2]),float(tokens[3])))
            elif tokens[0] == 'vn':
                group['normals'].append(QVector3D(float(tokens[1]),float(tokens[2]),float(tokens[3])))
            elif tokens[0] == 'f':
                group['faces'].append(map(lambda token: int(token.split('/')[0])-1, tokens[1:]))
        f.close()

        parts = PrimitiveParts()
        parts.setNumVertices(len(group['vertices']))
        for i,point in enumerate(group['vertices']):
            parts.setVertex(i,point)
#        parts.setPoints(group['vertices'])

        parts.setNumFaces(len(group['faces']))
        for i,face in enumerate(group['faces']):
            parts.setFace(i,face)
#        parts.setFaces(group['faces'])

        mesh = Mesh.buildByIndex(parts)
        scene.addAsset(mesh)

MeshImporter.register(ObjImporter())

