#include "Space.h"
#include <QDebug>
#include <iostream>
#include <fstream>

using namespace std;


/* Zone imaginaire permettant de représenter une grille 3D */

Space::Space(MyMesh* _mesh)
{
    this->_mesh = _mesh;
}


void Space::InitXYZ()
// initialisation des valeurs Max et Min
{
    VertexHandle initVertex = *(_mesh->vertices_begin());
    OpenMesh::Vec3f initPoint = _mesh->point(initVertex);

    xMax = initPoint[0], xMin = initPoint[0];
    yMax = initPoint[1], yMin = initPoint[1];
    zMax = initPoint[2], zMin = initPoint[2];
}


void Space::CreateSpace()
/*Créer l'obj permettant de voir le quadrillage*/
{
    InitXYZ();

    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++) {
        VertexHandle current = *curVert;
        OpenMesh::Vec3f point = _mesh->point(current);

        if(point[0] < xMin)
            xMin = point[0];
        if(point[0] > xMax)
            xMax = point[0];

        if(point[1] < yMin)
            yMin = point[1];
        if(point[1] > yMax)
            yMax = point[1];

        if(point[2] < zMin)
            zMin = point[2];
        if(point[2] > zMax)
            zMax = point[2];
    }
    qDebug() << "valeur de xMax :" << xMax << " xMin :" << xMin << " yMax :" << yMax << " yMin :" << yMin
             << " zMax :" << zMax << " zMin :" << zMin;


    int nombrePoint = 8;
    int haut = 2;
    int lon = 2;
    int lar = 3;
    vector<OpenMesh::Vec3f> points;
    OpenMesh::Vec3f point000 = {xMin,yMin,zMin};
    OpenMesh::Vec3f pointinter = {xMin,yMax/2.0,zMin};
    OpenMesh::Vec3f point010 = {xMin,yMax,zMin};
    OpenMesh::Vec3f point110 = {xMax,yMax,zMin};
    OpenMesh::Vec3f pointinter2 = {xMax,yMax/2.0,zMin};
    OpenMesh::Vec3f point100 = {xMax,yMin,zMin};

    OpenMesh::Vec3f point001 = {xMin,yMin,zMax};
    OpenMesh::Vec3f point011 = {xMin,yMax,zMax};
    OpenMesh::Vec3f point111 = {xMax,yMax,zMax};
    OpenMesh::Vec3f point101 = {xMax,yMin,zMax};

    points = {point000, pointinter, point010, point100, pointinter2 ,point110, point001, point011, point101, point111};

    ofstream myfile;
    myfile.open ("example.obj");

    for(auto i : points){
        myfile << "v " << i << "\n";
    }


    for(int l = 1; l<lar ;l++){
       for(int lo = 1; lo < lon ; lo++){
           myfile << "f " << l << " " << l+lar << " " << l+lar+1 << "\n";
           myfile << "f " << l << " " << l+lar+1 << " " << l+1 << "\n";
       }
    }

/*
    myfile << "f 1 2 3 \n";
    myfile << "f 1 4 3 \n";

    myfile << "f 1 2 6 \n";
    myfile << "f 1 5 6 \n";

    myfile << "f 5 6 7 \n";
    myfile << "f 5 8 7 \n";

    myfile << "f 4 3 7 \n";
    myfile << "f 4 8 7 \n";

    myfile << "f 1 4 8 \n";
    myfile << "f 1 5 8 \n";

    myfile << "f 2 6 7 \n";
    myfile << "f 2 3 7 \n";*/
    myfile.close();

}
