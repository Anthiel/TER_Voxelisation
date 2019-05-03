#ifndef SPACE_H
#define SPACE_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "mainwindow.h"

using namespace OpenMesh;
using namespace OpenMesh::Attributes;

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class Space
{
public:
    Space(MyMesh* _mesh);
    void CreateSpace();
    void InitXYZ();

public:
    MyMesh* _mesh;

    float xMax, xMin;
    float yMax, yMin;
    float zMax, zMin;

    float largeur;
    float hauteur;
    float longueur;

    float Lvoxel; // largeur d'un voxel
};

#endif // SPACE_H
