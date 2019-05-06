#ifndef SPACE_H
#define SPACE_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <iostream>

#include "mainwindow.h"

using namespace OpenMesh;
using namespace OpenMesh::Attributes;
using namespace std;

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class Space
{
public:
    Space(MyMesh* _mesh);
    void CreateSpace();
    void InitXYZ();
    void ChangeSize(int la, int lo, int ha);
    int coefficientVoxel(int index);
    int hauteurVoxel(int index);

    void CreateCube(int index, ofstream &file);
    int is_in(OpenMesh::Vec3f s, int index);
    int appearCube(int index, ofstream &file);
    void GenerePoints(int haut, int lon, int lar);


public:
    MyMesh* _mesh;

    float xMax, xMin;
    float yMax, yMin;
    float zMax, zMin;
    vector<OpenMesh::Vec3f> points;

    int largeur; // nombre de voxel en largeur
    int hauteur; // nombre de voxel en hauteur
    int longueur; // nombre de voxel en longueur
    int nbVoxel;
    int complex = 20;
    float Lvoxel; // largeur d'un voxel



};

#endif // SPACE_H