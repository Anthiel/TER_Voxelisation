#ifndef SPACE_H
#define SPACE_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <iostream>
#include <fstream>

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
    void BuildCubeCoord();

    int BiggestCoord(float c1, float c2, float c3);

    void ChangeSize(int la, int lo, int ha);
    void ChangeSize(int nb);

    int coefficientVoxel(int index);
    int hauteurVoxel(int index);

    void VoxelisationVertice(std::vector<int> &v);

    void CreateAllVoxel(std::ofstream &file);
    void DeleteDuplicate(std::vector<int> &v);

    void CreateCube(int index, std::ofstream &file);

    std::vector<OpenMesh::Vec3f> GenerePoints(int haut, int lon, int lar);


public:
    MyMesh* _mesh;

    float xMax, xMin;
    float yMax, yMin;
    float zMax, zMin;

    int largeur; // nombre de voxel en largeur
    int hauteur; // nombre de voxel en hauteur
    int longueur; // nombre de voxel en longueur
    int nbVoxel;

    std::vector<int> activatedVoxel;

    float Lvoxel; // largeur d'un voxel
};

#endif // SPACE_H
