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

vector<OpenMesh::Vec3f> Space::GenerePoints(int haut, int lon, int lar){
/* Genere des points alignés dans un rectangle avec distance égale*/
    vector<OpenMesh::Vec3f> points;
    for(int ha = 0; ha<=haut; ha++){
        for(int lo = 0; lo<= lon; lo++){
            for(int la = 0; la<=lar; la++){
                qDebug() << "point : "<< (la/lar) << (lo/lon) << (ha/haut);
                OpenMesh::Vec3f pointTMP = {xMin+(float(la)/float(lar))*(xMax-xMin), yMin+(float(lo)/float(lon))*(yMax-yMin), zMin+(float(ha)/float(haut))*(zMax-zMin)};
                points.push_back(pointTMP);
            }
        }
    }
    return points;
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


    /*
     * Precision du voxelisateur, nombre de cube présent en hauteur, Longueur et largeur
     */
    int haut = 15;
    int lon = 15;
    int lar = 15;

    //génère les points avec des distances égales en hauteur, longueur et largeur
    vector<OpenMesh::Vec3f> points = GenerePoints(haut-1, lon-1, lar-1);

    //création du fichier obj
    ofstream myfile;
    myfile.open ("example.obj");

    //écriture de tous les vertices dans l'obj
    for(auto i : points){
        myfile << "v " << i << "\n";
    }

    //variable permettant de connaitre l'étage, et la distance qu'il y a entre chaque étage
    int Etage = 0, EtageSuivant = 0;

    for(int h = 0; h<haut; h++){
        for(int la = 0; la < lar ;la++){
           for(int lo = 0; lo < lon; lo++){

               EtageSuivant = lar*lon; //taille d'un étage
               Etage = h*EtageSuivant;

               //plan XZ
               if(la<lar-1 && lo<lon-1){
                   myfile << "f " << 1 + la + lo*lar +Etage<< " " << 2 + la + lo*lar +Etage<< " " << 1 + la + lo*lar + lar +Etage<< "\n";
                   myfile << "f " << 2 + la + lo*lar + lar +Etage<< " " << 2 + la + lo*lar +Etage<< " " << 1 + la + lo*lar + lar +Etage<< "\n";
               }
               // PLAN XY
               if(h<haut-1 && lo<lon-1){
                    myfile << "f " << 1 + la + lo*lar +Etage<< " " << 1 + la + lo*lar +Etage+EtageSuivant<< " " << 1 + la + lo*lar + lar +Etage<< "\n";
                    myfile << "f " << 1 + la + lo*lar +Etage+EtageSuivant<< " " <<  1 + la + lo*lar + lar +Etage+EtageSuivant<< " " << 1 + la + lo*lar + lar +Etage<< "\n";
               }
               // PLAN YZ
               if(la<lar-1 && h<haut-1){
                    myfile << "f " << 1 + la + lo*lar +Etage<< " " << 2 + la + lo*lar +Etage<< " " << 1 + la + lo*lar +Etage+EtageSuivant<< "\n";
                    myfile << "f " << 2 + la + lo*lar +Etage+EtageSuivant<< " " << 2 + la + lo*lar +Etage<< " " << 1 + la + lo*lar +Etage+EtageSuivant<< "\n";
               }
           }
        }
    }
    myfile.close();
}
