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

    OpenMesh::Vec3f point000 = {xMin,yMin,zMin};
    OpenMesh::Vec3f point010 = {xMin,yMax,zMin};
    OpenMesh::Vec3f point110 = {xMax,yMax,zMin};
    OpenMesh::Vec3f point100 = {xMax,yMin,zMin};

    OpenMesh::Vec3f point001 = {xMin,yMin,zMax};
    OpenMesh::Vec3f point011 = {xMin,yMax,zMax};
    OpenMesh::Vec3f point111 = {xMax,yMax,zMax};
    OpenMesh::Vec3f point101 = {xMax,yMin,zMax};

    ofstream myfile;
    myfile.open ("example.obj");
    myfile << "v " << point000 << "\n"; //1
    myfile << "v " << point010 << "\n"; //2
    myfile << "v " << point110 << "\n"; //3
    myfile << "v " << point100 << "\n"; //4

    myfile << "v " << point001 << "\n"; //5
    myfile << "v " << point011 << "\n"; //6
    myfile << "v " << point111 << "\n"; //7
    myfile << "v " << point101 << "\n"; //8

    myfile << "f 1 2 3 \n";
    myfile << "f 1 3 4 \n";

    myfile << "f 1 2 6 \n";
    myfile << "f 1 6 5 \n";

    myfile << "f 5 6 7 \n";
    myfile << "f 5 7 8 \n";

    myfile << "f 4 3 7 \n";
    myfile << "f 4 7 8 \n";

    myfile << "f 1 8 4 \n";
    myfile << "f 1 5 8 \n";

    myfile << "f 2 6 7 \n";
    myfile << "f 2 7 3 \n";
    myfile.close();

}
