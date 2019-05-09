#include "Space.h"
#include <QDebug>

/* Zone imaginaire permettant de représenter une grille 3D */

Space::Space(MyMesh* _mesh)
{
    this->_mesh = _mesh;
    ChangeSize(10+1, 10+1, 10+1); //30 voxels de large
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

int Space::BiggestCoord(float c1, float c2, float c3){
    /* renvoie l'indice de la plus grande valeur */

    if(c1 >= c2 && c1 >= c3){
        return 1;
    }
    if(c2 >= c3 && c2 >= c1){
        return 2;
    }
    if(c3 >= c1 && c3 >= c2){
        return 3;
    }
    return 0;
}

void Space::BuildCubeCoord(){
/* transforme le Space en cube */

    float Cx = xMax - xMin;
    float Cy = yMax - yMin;
    float Cz = zMax - zMin;

    int result = BiggestCoord(Cx, Cy, Cz);
    if(result ==0){
        qDebug() << "error : BuildCubeCoord : mauvaise valeur renvoyé";
    }
    else if(result == 1){
        float Dxy = Cx-Cy;
        yMax = yMax + Dxy/2.0;
        yMin = yMin - Dxy/2.0;
        float Dxz = Cx-Cz;
        zMax = zMax + Dxz/2.0;
        zMin = zMin - Dxz/2.0;
    }
    else if(result == 2){
        float Dyx = Cy-Cx;
        xMax = xMax + Dyx/2.0;
        xMin = xMin - Dyx/2.0;
        float Dyz = Cy-Cz;
        zMax = zMax + Dyz/2.0;
        zMin = zMin - Dyz/2.0;
    }
    else if(result == 3){
        float Dzx = Cz-Cx;
        xMax = xMax + Dzx/2.0;
        xMin = xMin - Dzx/2.0;
        float Dzy = Cz-Cy;
        yMax = yMax + Dzy/2.0;
        yMin = yMin - Dzy/2.0;
    }
    Cx = xMax - xMin;
    Cy = yMax - yMin;
    Cz = zMax - zMin;
    qDebug() << "xMax/Min : " << xMax << xMin << "yMax/Min : " << yMax << yMin << "zMax/Min : " << zMax << zMin;
}

std::vector<OpenMesh::Vec3f> Space::GenerePoints(int haut, int lon, int lar){
/* Genere des points alignés dans un rectangle avec distance égale*/
    std::vector<OpenMesh::Vec3f> points;
    for(int ha = 0; ha<=haut; ha++){
        for(int lo = 0; lo<= lon; lo++){
            for(int la = 0; la<=lar; la++){
                OpenMesh::Vec3f pointTMP = {xMin+(float(la)/float(lar))*(xMax-xMin), yMin+(float(lo)/float(lon))*(yMax-yMin), zMin+(float(ha)/float(haut))*(zMax-zMin)};
                points.push_back(pointTMP);
            }
        }
    }
    return points;
}

void Space::ChangeSize(int la, int lo, int ha){
/* permet de changer le nombre de voxel en hauteur, largeur et longueur*/

    largeur = la;
    hauteur = ha;
    longueur = lo;
    nbVoxel = (la-1)*(ha-1)*(lo-1);
}
void Space::ChangeSize(int nb){
/* permet de changer le nombre de voxel en hauteur, largeur et longueur*/

    largeur = nb;
    hauteur = nb;
    longueur = nb;
    nbVoxel = (nb-1)*(nb-1)*(nb-1);
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

OpenMesh::Vec3f Space::GetVoxelCoord(int VoxelID){
    int larg = largeur - 1;
    int longu = longueur - 1;
    int haut = hauteur - 1;
    int Etage = longu * larg;

    int v = VoxelID;

    int h = (v-1)/Etage + 1 ;
    v = v - ((h-1) * Etage);
    int lon = (v-1) / larg + 1;
    v = v - ((lon-1)*larg);
    int lar = v;
    OpenMesh::Vec3f point = {lon,lar,h};
    return point;
}

int Space::getVoxelIndex(int VertexID){

    int lar = largeur - 1;
    int lon = longueur - 1;
    int haut = hauteur - 1;

    VertexHandle current = _mesh->vertex_handle(VertexID);
    OpenMesh::Vec3f point = _mesh->point(current);
    float x = point[0], y = point[1], z = point[2];
    int currentLa = 0, currentLon = 0, currentHau = 0;

    for(int la = 0; la < lar; la++){
        float p = xMin+(float(la)/float(lar))*(xMax-xMin);
        float p1 = xMin+(float(la+1)/float(lar))*(xMax-xMin);
        if(p <= x && x <= p1){
             currentLa = la;
             break;
        }
    }
    for(int lo = 0; lo < lon; lo++){
        float p = yMin+(float(lo)/float(lon))*(yMax-yMin);
        float p1 = yMin+(float(lo+1)/float(lon))*(yMax-yMin);
        if(p <= y && y <= p1){
             currentLon = lo;
             break;
        }
    }
    for(int ha = 0; ha < haut; ha++){
        float p = zMin+(float(ha)/float(haut))*(zMax-zMin);
        float p1 = zMin+(float(ha+1)/float(haut))*(zMax-zMin);
        if(p <= z && z <= p1){
             currentHau = ha;
             break;
        }
    }

    int Etage = lon * lar;
    return currentLa + currentLon*lar+Etage*currentHau + 1;
}



int Space::getVoxelIndex(int lo, int lar, int hau){
    int larg = largeur - 1;
    int longu = longueur - 1;
    int haut = hauteur - 1;
    int Etage = longu * larg;

    return (lar+(lo-1)*(larg)+Etage*(hau-1));
}

void Space::MoyenneVoxel(std::vector<int> &v, OpenMesh::Vec3f V1coord, OpenMesh::Vec3f V2coord){

    int V1index = getVoxelIndex(V1coord[0], V1coord[1], V1coord[2]);
    int V2index = getVoxelIndex(V2coord[0], V2coord[1], V2coord[2]);

    float res1 = (V1coord[0]+V2coord[0])/2.0;
    float res2 = (V1coord[1]+V2coord[1])/2.0;
    float res3 = (V1coord[2]+V2coord[2])/2.0;

    float res21 = res1;
    float res22 = res2;
    float res23 = res3;

    if(res1-float(int(res1)) != 0){
        res1 = float(int(res1))+1;
        res21 = res1 - 1;
    }
    if(res2-float(int(res2)) != 0){
        res2 = float(int(res2))+1;
        res22 = res2 - 1;
    }
    if(res3-float(int(res3)) != 0){
        res3 = float(int(res3))+1;
        res23 = res3 - 1;
    }

    OpenMesh::Vec3f VmoyenneCoord = {res1, res2, res3};
    OpenMesh::Vec3f VmoyenneCoord2 = {res21, res22, res23};


    int VmoyenneIndex = getVoxelIndex(VmoyenneCoord[0], VmoyenneCoord[1], VmoyenneCoord[2]);
    int VmoyenneIndex2 = getVoxelIndex(VmoyenneCoord2[0], VmoyenneCoord2[1], VmoyenneCoord2[2]);


    if(V1index == VmoyenneIndex || V2index == VmoyenneIndex || V1index == VmoyenneIndex2 || V2index == VmoyenneIndex2){
        return;
    }
    v.push_back(VmoyenneIndex);
    v.push_back(VmoyenneIndex2);

    MoyenneVoxel(v, V1coord, VmoyenneCoord2);
    MoyenneVoxel(v, VmoyenneCoord, V2coord);
}


void Space::VoxelisationVertice(std::vector<int> &v){

    // parcours de tous les sommets
    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++) {

        VertexHandle current = *curVert;
        int voxelID = getVoxelIndex(current.idx());
        v.push_back(voxelID);
    }
}


void Space::VoxelisationEdge(std::vector<int> &v){
    // parcours de toutes les arêtes
    int lar = largeur - 1;
    int lon = longueur - 1;
    int haut = hauteur - 1;

    // parcours des arêtes
    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
    {
        EdgeHandle eh = *curEdge;

        VertexHandle S1 = _mesh->to_vertex_handle(_mesh->halfedge_handle(eh, 0));
        VertexHandle S2 = _mesh->to_vertex_handle(_mesh->halfedge_handle(eh, 1));

        int Voxel1 = getVoxelIndex(S1.idx());
        int Voxel2 = getVoxelIndex(S2.idx());
        v.push_back(Voxel1);
        v.push_back(Voxel2);

        OpenMesh::Vec3f V1coord = GetVoxelCoord(Voxel1);
        OpenMesh::Vec3f V2coord = GetVoxelCoord(Voxel2);

        MoyenneVoxel(v, V1coord, V2coord);
    }
}

void Space::VoxelisationFace(std::vector<int> &v){
    // parcours de toutes les arêtes
    int lar = largeur - 1;
    int lon = longueur - 1;
    int haut = hauteur - 1;

    // parcours des faces
    for (MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++)
    {
        std::vector<int> points;
        FaceHandle fh = *curFace;
        for (MyMesh::FaceVertexIter curVert = _mesh->fv_iter(*curFace); curVert.is_valid(); curVert ++)
        {
                points.push_back(getVoxelIndex((*curVert).idx()));
        }

        v.push_back(points[0]);     v.push_back(points[1]);     v.push_back(points[2]);

        std::vector<int> listpointBase;

        OpenMesh::Vec3f Sommet = GetVoxelCoord(points[0]);
        OpenMesh::Vec3f V1coord = GetVoxelCoord(points[1]);
        OpenMesh::Vec3f V2coord = GetVoxelCoord(points[2]);

        MoyenneVoxel(v, Sommet, V1coord);
        MoyenneVoxel(v, Sommet, V2coord);
        MoyenneVoxel(v, V1coord, V2coord);
        MoyenneVoxel(listpointBase, V1coord, V2coord);

        for(auto i : listpointBase){
            OpenMesh::Vec3f ViCoord = GetVoxelCoord(i);
            MoyenneVoxel(v, Sommet, ViCoord);
        }
    }
}


void Space::CreateCube(int index, std::ofstream &file){
/* permet de dessiner le voxel à l'index choisie*/

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << "error : Space::CreateCube : dépassement du nombre de voxel total, index : " << index << "pour un maximum de : " << (longueur-1)*(hauteur-1)*(largeur-1);
        return;
    }

    int EtageSuivant = longueur*largeur;
    int nbVoxelLargeur = largeur-1;
    int nbVoxelLongueur = longueur-1;
    int nbVoxelParEtage = nbVoxelLargeur * nbVoxelLongueur;

    //sur une pile de voxel, voxelRDC est l'index du voxel le plus bas de la pile
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

void Space::DeleteDuplicate(std::vector<int> &v){
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end() ), v.end());
}

void Space::CreateAllVoxel(std::ofstream &file){
    for(auto i : activatedVoxel)
        CreateCube(i, file);
}

void Space::fillWithVoxel(std::vector<int> &v){
    //supprime les valeurs doubles
    DeleteDuplicate(activatedVoxel);
    std::vector<int> newVoxel;
    for(int i = 0; i<v.size();i++){
        qDebug() << "valeur actuelle de i :" << i;
        for(int k = v.at(i); k<= k+(largeur-(k%largeur)) ; k++){
            if(std::find(v.begin(), v.end(), k) != v.end()){
                qDebug() << "K trouvé :" << k;

                for(int j = v.at(i); j<=k ; j++){
                    newVoxel.push_back(j);
                }
                i++;
                break;
            }
        }
    }
    qDebug() << "newVoxel :" << newVoxel;
    qDebug() << "v :" << v;
    std::vector<int> TotalVoxel;
    TotalVoxel.reserve( newVoxel.size() + v.size() ); // preallocate memory
    TotalVoxel.insert( TotalVoxel.end(), newVoxel.begin(), newVoxel.end() );
    TotalVoxel.insert( TotalVoxel.end(), v.begin(), v.end() );
    v = TotalVoxel;
    qDebug() << "v combiné:" << v;
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

    // Créer le space carré, pour avoir des voxels cubiques
    BuildCubeCoord();

    // Precision du voxelisateur, nombre de cube présent en hauteur, Longueur et largeur
    int haut = hauteur;
    int lon = longueur;
    int lar = largeur;

    //génère les points avec des distances égales en hauteur, longueur et largeur
    std::vector<OpenMesh::Vec3f> points = GenerePoints(haut-1, lon-1, lar-1);

    //création du fichier obj
    std::ofstream myfile;
    myfile.open ("example.obj");

    //écriture de tous les vertices dans l'obj
    for(auto i : points){
        myfile << "v " << i << "\n";
    }

    qDebug() << " passage à la voxelisation ";
    //Voxelisation avec les vertices
    /*VoxelisationVertice(activatedVoxel);*/

    //Voxelisation avec les edges
    /*VoxelisationEdge(activatedVoxel);*/

    //Voxelisation avec les faces
    VoxelisationFace(activatedVoxel);

    //remplissage intérieur
    fillWithVoxel(activatedVoxel);

    qDebug() << " fin de la voxelisation ";

    //supprime les valeurs doubles
    DeleteDuplicate(activatedVoxel);

    //Dessine les voxels activés
    CreateAllVoxel(myfile);


    myfile.close();


}
