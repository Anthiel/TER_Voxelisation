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
    enum Voxelisation { VoxelisationByVertice, VoxelisationByEdge, VoxelisationByFace };
    Space(MyMesh* _mesh, Voxelisation voxelisationType, int size);
    void createSpace();
    void initXYZ();
    void changeSize(int la, int lo, int ha);
    void changeSize(int nb);
    void buildCubeCoord();
    int biggestCoord(float c1, float c2, float c3);
    void generatePoints(int haut, int lon, int lar);
    void createAllVoxel(QString filename);
    void deleteDuplicate();

    void voxelize(QString fileName);
    void voxelisationVertice();
    void voxelisationEdge();
    void voxelisationFace();
    void fillWithVoxels();

    int coefficientVoxel(int index);
    int hauteurVoxel(int index);
    OpenMesh::Vec3f getVoxelCoord(int VoxelID);
    int getVoxelIndex(int VertexID);
    int getVoxelIndex(int lo, int lar, int hau);
    void moyenneVoxel(std::vector<int> &v, OpenMesh::Vec3f V1coord, OpenMesh::Vec3f V2coord);
    void createCube(int index, std::ofstream &file);
    int getTotalVoxels();

    bool checkWallLongueur(int VoxelID);
    bool checkWallLargeur(int VoxelID);
    bool checkWallHauteur(int VoxelID);
    bool isInsideMesh(int VoxelID);
    void getBoundaries();
    void fillBoundaries();



private:
    MyMesh* _mesh;

    float xMax, xMin;
    float yMax, yMin;
    float zMax, zMin;

    int largeur; // nombre de voxel en largeur
    int hauteur; // nombre de voxel en hauteur
    int longueur; // nombre de voxel en longueur
    int nbVoxel;

    std::vector<int> activatedVoxel;
    std::vector<OpenMesh::Vec3f> _points;
    std::vector<std::vector<VertexHandle>> boundariesVertex;

    float Lvoxel; // largeur d'un voxel

    Voxelisation voxelisationType;
};

#endif // SPACE_H
