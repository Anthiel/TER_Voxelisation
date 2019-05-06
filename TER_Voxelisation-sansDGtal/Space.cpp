#include "Space.h"
#include <QDebug>
#include <iostream>
#include <fstream>

using namespace std;


/* Zone imaginaire permettant de représenter une grille 3D */

Space::Space(MyMesh* _mesh)
{
    this->_mesh = _mesh;
    ChangeSize(14, 14, 15);
}


void Space::InitXYZ()
// initialisation des valeurs Max et Min
{
    VertexHandle initVertex = *(_mesh->vertices_begin());
    OpenMesh::Vec3f initPoint = _mesh->point(initVertex);

    xMax = initPoint[0], xMin = initPoint[0];
    yMax = initPoint[1], yMin = initPoint[1];
    zMax = initPoint[2], zMin = initPoint[2];

    vector<OpenMesh::Vec3f> pt_init;
    points = pt_init;
}

void Space::GenerePoints(int haut, int lon, int lar){
/* Genere des points alignés dans un rectangle avec distance égale*/
    for(int ha = 0; ha<=haut; ha++){
        for(int lo = 0; lo<= lon; lo++){
            for(int la = 0; la<=lar; la++){
                OpenMesh::Vec3f pointTMP = {xMin+(float(la)/float(lar))*(xMax-xMin), zMin+(float(ha)/float(haut))*(zMax-zMin), yMin+(float(lo)/float(lon))*(yMax-yMin)};
                points.push_back(pointTMP);
            }
        }
    }
}

void Space::ChangeSize(int la, int lo, int ha){
/* permet de changer le nombre de voxel en hauteur, largeur et longueur*/

    largeur = la;
    hauteur = ha;
    longueur = lo;
    nbVoxel = (la-1)*(ha-1)*(lo-1);
}

int Space::coefficientVoxel(int index){
/* renvoie la valeur qu'il faut rajouter à l'index du voxel pour avoir l'origine */

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << "error : Space::coefficientVoxel : dépassement du nombre de voxel total";
        return -1;
    }

    int nbVoxelLargeur = largeur - 1;
    int coef = 0;

    for(int i=0; i<index;i++){
        if((index-i)%nbVoxelLargeur == 1){
            coef = index-1;
            break;
        }
    }
    return coef/nbVoxelLargeur;
}

int Space::hauteurVoxel(int index){
    /*renvoie la hauteur dans la structure du voxel n°index*/

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << "error : Space::hauteurVoxel : dépassement du nombre de voxel total";
        return -1;
    }

    int nbVoxelLargeur = largeur-1;
    int nbVoxelLongueur = longueur-1;
    int nbVoxelParEtage = nbVoxelLargeur * nbVoxelLongueur;
    return (index-1)/nbVoxelParEtage;
}

void Space::CreateCube(int index, ofstream &file){
/* permet de dessiner le voxel à l'index choisie*/

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << " error : Space::CreateCube : " << index << " dépassement du nombre de voxel total";
        return;
    }

    int EtageSuivant = longueur*largeur;
    int nbVoxelLargeur = largeur-1;
    int nbVoxelLongueur = longueur-1;
    int nbVoxelParEtage = nbVoxelLargeur * nbVoxelLongueur;

    int voxelRDC = index - hauteurVoxel(index)*nbVoxelParEtage;

    /*
     * Beh = behind
     * Fro = front
    */

    //Etage 0 du voxel
    int vertexFroLeftBottom = voxelRDC + coefficientVoxel(voxelRDC) + (hauteurVoxel(index)*EtageSuivant);
    int vertexFroRightBottom = vertexFroLeftBottom + largeur;
    int vertexBehLeftBottom = vertexFroLeftBottom + 1;
    int vertexBehRightBottom = vertexBehLeftBottom + largeur;

    //Etage 1 du voxel
    int vertexFroLeftUp = vertexFroLeftBottom + EtageSuivant;
    int vertexFroRightUp = vertexFroRightBottom + EtageSuivant;
    int vertexBehLeftUp = vertexBehLeftBottom + EtageSuivant;
    int vertexBehRightUp = vertexBehRightBottom + EtageSuivant;

    /* écriture des 6 faces du carré dans le fichier obj créé*/
    //face
    file << "f " << vertexFroLeftBottom << " " << vertexFroLeftUp << " " << vertexFroRightBottom << "\n";
    file << "f " << vertexFroRightUp << " " << vertexFroLeftUp << " " << vertexFroRightBottom << "\n";
    //derrière
    file << "f " << vertexBehLeftBottom << " " << vertexBehLeftUp << " " << vertexBehRightBottom << "\n";
    file << "f " << vertexBehRightUp << " " << vertexBehLeftUp << " " << vertexBehRightBottom << "\n";
    //haut
    file << "f " << vertexFroLeftUp << " " << vertexBehLeftUp << " " << vertexFroRightUp << "\n";
    file << "f " << vertexBehRightUp << " " << vertexBehLeftUp << " " << vertexFroRightUp << "\n";
    //bas
    file << "f " << vertexFroLeftBottom << " " << vertexBehLeftBottom << " " << vertexFroRightBottom << "\n";
    file << "f " << vertexBehRightBottom << " " << vertexBehLeftBottom << " " << vertexFroRightBottom << "\n";
    //gauche
    file << "f " << vertexBehLeftUp << " " << vertexFroLeftUp << " " << vertexFroLeftBottom << "\n";
    file << "f " << vertexBehLeftUp << " " << vertexBehLeftBottom << " " << vertexFroLeftBottom << "\n";
    //droite
    file << "f " << vertexBehRightUp << " " << vertexFroRightUp << " " << vertexFroRightBottom << "\n";
    file << "f " << vertexBehRightUp << " " << vertexBehRightBottom << " " << vertexFroRightBottom << "\n";
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
    int haut = hauteur;
    int lon = longueur;
    int lar = largeur;

    //génère les points avec des distances égales en hauteur, longueur et largeur
    GenerePoints(haut-1, lon-1, lar-1);

    //création du fichier obj
    ofstream myfile;
    myfile.open ("example.obj");

    //écriture de tous les vertices dans l'obj
    for(auto i : points){
        myfile << "v " << i << "\n";
    }

    /*CreateCube(2, myfile);
    CreateCube(8, myfile);
    CreateCube(14, myfile);
    CreateCube(23, myfile);
    CreateCube(32, myfile);
    CreateCube(41, myfile);
    CreateCube(100, myfile);
    CreateCube(225, myfile);*/

    for(int cube = 0; cube <= (longueur-1)*(largeur-1)*(hauteur-1); cube++){
        if(appearCube(cube, myfile) == 1){
            CreateCube(cube, myfile);
        }
    }


    /*
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
    }*/


    myfile.close();
}


int Space::is_in(OpenMesh::Vec3f s, int index){

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << index << " error : Space::is_in : dépassement du nombre de voxel total";
        return 0;
    }

    int EtageSuivant = longueur*largeur;
    int nbVoxelLargeur = largeur-1;
    int nbVoxelLongueur = longueur-1;
    int nbVoxelParEtage = nbVoxelLargeur * nbVoxelLongueur;

    int voxelRDC = index - hauteurVoxel(index)*nbVoxelParEtage;

    /*
     * Beh = behind
     * Fro = front
    */

    //Etage 0 du voxel
    int vertexFroLeftBottom = voxelRDC + coefficientVoxel(voxelRDC) + (hauteurVoxel(index)*EtageSuivant);
    int vertexFroRightBottom = vertexFroLeftBottom + largeur;
    int vertexBehLeftBottom = vertexFroLeftBottom + 1;
    int vertexBehRightBottom = vertexBehLeftBottom + largeur;

    //Etage 1 du voxel
    int vertexFroLeftUp = vertexFroLeftBottom + EtageSuivant;
    int vertexFroRightUp = vertexFroRightBottom + EtageSuivant;
    int vertexBehLeftUp = vertexBehLeftBottom + EtageSuivant;
    int vertexBehRightUp = vertexBehRightBottom + EtageSuivant;

    float x = s[0], y = s[1], z = s[2];
    if(x >= points[vertexFroLeftBottom][0] && x <= points[vertexBehLeftBottom][0] && z >= points[vertexFroLeftBottom][2] && z <= points[vertexFroRightBottom][2] && y >= points[vertexBehLeftBottom][1] && y <= points[vertexBehLeftUp][1]){
        return 1;
    }
    else
        return 0;
}


int Space::appearCube(int index, ofstream &file){

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << index << " error : Space::appearCube : dépassement du nombre de voxel total";
        return 0;
    }

    vector<OpenMesh::Vec3f> p1; //future liste des points sur une des arretes
    vector<OpenMesh::Vec3f> p2;//future liste des points sur une autre arrete

    OpenMesh::Vec3f s1 = _mesh->point(_mesh->vertex_handle(0));
    OpenMesh::Vec3f s2 = _mesh->point(_mesh->vertex_handle(1));
    OpenMesh::Vec3f s3 = _mesh->point(_mesh->vertex_handle(2));
    OpenMesh::Vec3f s4;
    OpenMesh::Vec3f s5;

    for(int i = 0; i<=complex; i++){
        s4 = (1-float(i)/complex)*s1 + (float(i)/complex)*s2;
        s5 = (1-float(i)/complex)*s1 + (float(i)/complex)*s3;
        p1.push_back(s4);
        p2.push_back(s5);
    }

    for(int i = 0; i<=complex+1; i++){
        for(int t = 0; t<=complex; t++){
            if(is_in((1-float(t)/complex)*p1[i] + (float(t)/complex)*p2[i], index) == 1){
                return 1;
            }
        }
    }
    return 0;

}
