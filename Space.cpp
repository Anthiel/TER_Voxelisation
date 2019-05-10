#include "Space.h"
#include <QDebug>

/* Zone imaginaire permettant de représenter une grille 3D */

Space::Space(MyMesh* _mesh, Voxelisation voxelisationType, int size){
    this->_mesh = _mesh;
    this->voxelisationType = voxelisationType;
    this->changeSize(size+1, size+1, size+1); // il y a size voxel
    this->initXYZ();

}

// Créer l'obj permettant de voir le quadrillage
void Space::createSpace(){

    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++) {
        VertexHandle current = *curVert;
        OpenMesh::Vec3f point = _mesh->point(current);

        if(point[0] < xMin) xMin = point[0];
        if(point[0] > xMax) xMax = point[0];

        if(point[1] < yMin) yMin = point[1];
        if(point[1] > yMax) yMax = point[1];

        if(point[2] < zMin) zMin = point[2];
        if(point[2] > zMax) zMax = point[2];
    }

    // Créer le space carré, pour avoir des voxels cubiques
    this->buildCubeCoord();

    //génère les points avec des distances égales en hauteur, longueur et largeur
    this->generatePoints(hauteur-1, longueur-1, largeur-1);
}

// Fonction de voxélisation
void Space::voxelize(QString fileName){

    //création du fichier obj
    std::ofstream meshObj;
    meshObj.open(fileName.toStdString());

    //écriture de tous les vertices dans l'obj
    for(auto i : _points){
        meshObj << "v " << i << "\n";
    }

    qDebug() << "[DEBUG]" << "Début de la voxélisation";

    switch(voxelisationType){
        case VoxelisationByVertice:
            voxelisationVertice();
        break;
        case VoxelisationByEdge:
            voxelisationEdge();
        break;
        case VoxelisationByFace:
            voxelisationFace();
        break;
    }
    this->fillWithVoxels();

    qDebug() << "[DEBUG]" << "Fin de la voxélisation";

    deleteDuplicate();

    //Dessine les voxels activés
    createAllVoxel(meshObj);

    meshObj.close();
}

// initialisation des valeurs Max et Min
void Space::initXYZ(){
    VertexHandle initVertex = *(_mesh->vertices_begin());
    OpenMesh::Vec3f initPoint = _mesh->point(initVertex);

    xMax = initPoint[0], xMin = initPoint[0];
    yMax = initPoint[1], yMin = initPoint[1];
    zMax = initPoint[2], zMin = initPoint[2];
}

// renvoie l'indice de la plus grande valeur
int Space::biggestCoord(float c1, float c2, float c3){
    if(c1 >= c2 && c1 >= c3) return 1;
    if(c2 >= c3 && c2 >= c1) return 2;
    if(c3 >= c1 && c3 >= c2) return 3;
    return 0;
}

// transforme le Space en cube
void Space::buildCubeCoord(){

    float Cx = xMax - xMin;
    float Cy = yMax - yMin;
    float Cz = zMax - zMin;

    unsigned result = biggestCoord(Cx, Cy, Cz);
    if(result == 0){
        qDebug() << "[ERROR]" << __FUNCTION__ << "Mauvaise valeur renvoyée";
    } else if(result == 1){
        float Dxy = Cx-Cy;
        yMax = yMax + Dxy/2.0;
        yMin = yMin - Dxy/2.0;
        float Dxz = Cx-Cz;
        zMax = zMax + Dxz/2.0;
        zMin = zMin - Dxz/2.0;
    } else if(result == 2) {
        float Dyx = Cy-Cx;
        xMax = xMax + Dyx/2.0;
        xMin = xMin - Dyx/2.0;
        float Dyz = Cy-Cz;
        zMax = zMax + Dyz/2.0;
        zMin = zMin - Dyz/2.0;
    } else if(result == 3) {
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
    qDebug() << "[DEBUG]" << "xMax/Min:" << xMax << xMin << "yMax/Min:" << yMax << yMin << "zMax/Min:" << zMax << zMin;
}

// Génere des points alignés dans un rectangle avec distance égale
void Space::generatePoints(int haut, int lon, int lar){

    for(int ha = 0; ha<=haut; ha++){
        for(int lo = 0; lo<= lon; lo++){
            for(int la = 0; la<=lar; la++){
                OpenMesh::Vec3f pointTMP = {
                    xMin + (float(la) / float(lar)) * (xMax - xMin),
                    yMin + (float(lo) / float(lon)) *(yMax - yMin),
                    zMin + (float(ha) / float(haut)) *(zMax - zMin)
                };
                this->_points.push_back(pointTMP);
            }
        }
    }
}

// permet de changer le nombre de voxel en hauteur, largeur et longueur
void Space::changeSize(int la, int lo, int ha){


    this->largeur = la;
    this->hauteur = ha;
    this->longueur = lo;
    this->nbVoxel = (la-1)*(ha-1)*(lo-1);
}

// permet de changer le nombre de voxel en hauteur, largeur et longueur
void Space::changeSize(int nb){


    this->largeur = nb;
    this->hauteur = nb;
    this->longueur = nb;
    this->nbVoxel = (nb-1)*(nb-1)*(nb-1);
}

// Voxélisation par vertex
void Space::voxelisationVertice(){

    // parcours de tous les sommets
    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++) {
        VertexHandle current = *curVert;
        int voxelID = getVoxelIndex(current.idx());
        activatedVoxel.push_back(voxelID);
    }
}

// Voxélisation par edge
void Space::voxelisationEdge(){

    // parcours des arêtes
    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++) {
        EdgeHandle eh = *curEdge;

        VertexHandle S1 = _mesh->to_vertex_handle(_mesh->halfedge_handle(eh, 0));
        VertexHandle S2 = _mesh->to_vertex_handle(_mesh->halfedge_handle(eh, 1));

        int Voxel1 = getVoxelIndex(S1.idx());
        int Voxel2 = getVoxelIndex(S2.idx());
        activatedVoxel.push_back(Voxel1);
        activatedVoxel.push_back(Voxel2);

        OpenMesh::Vec3f V1coord = getVoxelCoord(Voxel1);
        OpenMesh::Vec3f V2coord = getVoxelCoord(Voxel2);

        moyenneVoxel(activatedVoxel, V1coord, V2coord);
    }
}

// Voxélisation par face
void Space::voxelisationFace(){

    // parcours des faces
    for (MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++) {
        std::vector<int> points;
        for (MyMesh::FaceVertexIter curVert = _mesh->fv_iter(*curFace); curVert.is_valid(); curVert ++) {
                points.push_back(getVoxelIndex((*curVert).idx()));
        }

        activatedVoxel.push_back(points[0]);
        activatedVoxel.push_back(points[1]);
        activatedVoxel.push_back(points[2]);

        std::vector<int> listpointBase;

        OpenMesh::Vec3f Sommet = getVoxelCoord(points[0]);
        OpenMesh::Vec3f V1coord = getVoxelCoord(points[1]);
        OpenMesh::Vec3f V2coord = getVoxelCoord(points[2]);

        moyenneVoxel(activatedVoxel, Sommet, V1coord);
        moyenneVoxel(activatedVoxel, Sommet, V2coord);
        moyenneVoxel(activatedVoxel, V1coord, V2coord);
        moyenneVoxel(listpointBase, V1coord, V2coord);

        for(auto i : listpointBase){
            OpenMesh::Vec3f ViCoord = getVoxelCoord(i);
            moyenneVoxel(activatedVoxel, Sommet, ViCoord);
        }
    }
}

// renvoie la valeur qu'il faut rajouter à l'index du voxel pour avoir l'origine
int Space::coefficientVoxel(int index){

    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << "[ERROR]" << __FUNCTION__ << "Dépassement du nombre de voxels total";
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

// renvoie la hauteur dans la structure du voxel n°index
int Space::hauteurVoxel(int index){
    if(index > (longueur-1)*(hauteur-1)*(largeur-1)){
        qDebug() << "[ERROR]" << __FUNCTION__ << "Dépassement du nombre de voxels total";
        return -1;
    }

    int nbVoxelLargeur = largeur-1;
    int nbVoxelLongueur = longueur-1;
    int nbVoxelParEtage = nbVoxelLargeur * nbVoxelLongueur;
    return (index-1)/nbVoxelParEtage;
}

OpenMesh::Vec3f Space::getVoxelCoord(int VoxelID){
    int larg = largeur - 1;
    int longu = longueur - 1;
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
    int Etage = longu * larg;

    return (lar+(lo-1)*(larg)+Etage*(hau-1));
}

void Space::moyenneVoxel(std::vector<int> &v, OpenMesh::Vec3f V1coord, OpenMesh::Vec3f V2coord){

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

    moyenneVoxel(v, V1coord, VmoyenneCoord2);
    moyenneVoxel(v, VmoyenneCoord, V2coord);
}

// permet de dessiner le voxel à l'index choisi
void Space::createCube(int index, std::ofstream &file){

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

void Space::deleteDuplicate(){
    std::sort(activatedVoxel.begin(), activatedVoxel.end());
    activatedVoxel.erase(unique(activatedVoxel.begin(), activatedVoxel.end()), activatedVoxel.end());
}
void Space::createAllVoxel(std::ofstream &file){
    for(auto i : activatedVoxel)
        createCube(i, file);
}

int Space::getTotalVoxels(){
    return activatedVoxel.size();
}

bool Space::checkWallHauteur(int VoxelID){

    int Etage = (longueur-1) * (largeur-1);

    for(int i = VoxelID + Etage ; i<=nbVoxel; i = i + Etage){
         if(std::find(activatedVoxel.begin(), activatedVoxel.end(), i) != activatedVoxel.end()){
             for(int j = VoxelID - Etage; j >= 0 ; j = j-Etage){
                 if(std::find(activatedVoxel.begin(), activatedVoxel.end(), j) != activatedVoxel.end()){
                     return true;
                 }
             }
         }
    }
    return false;
}

bool Space::checkWallLargeur(int VoxelID){

    int barriereHaute = VoxelID+((largeur-1)-(VoxelID%(largeur-1)));
    int barriereBasse = barriereHaute - (largeur-1)+1;

    for(int i = VoxelID + 1 ; i<=barriereHaute; i = i + 1){
         if(std::find(activatedVoxel.begin(), activatedVoxel.end(), i) != activatedVoxel.end()){
             for(int j = VoxelID - 1; j >= barriereBasse ; j = j-1){
                 if(std::find(activatedVoxel.begin(), activatedVoxel.end(), j) != activatedVoxel.end()){
                     return true;
                 }
             }
         }
    }
    return false;
}

bool Space::checkWallLongueur(int VoxelID){

    int barriereHaute = nbVoxel;
    int barriereBasse = 0;
    int longu = longueur-1;

    for(int i = VoxelID + longu ; i<=barriereHaute; i = i + longu){
         if(std::find(activatedVoxel.begin(), activatedVoxel.end(), i) != activatedVoxel.end()){
             for(int j = VoxelID - longu; j >= barriereBasse ; j = j-longu){
                 if(std::find(activatedVoxel.begin(), activatedVoxel.end(), j) != activatedVoxel.end()){
                     return true;
                 }
             }
         }
    }
    return false;
}

bool Space::isItInsideTheMesh(int VoxelID){
    if(checkWallLargeur(VoxelID))
        if(checkWallLongueur(VoxelID))
            if(checkWallHauteur(VoxelID))
                return true;
    return false;
}

void Space::fillWithVoxels(){
    //supprime les valeurs doubles
    deleteDuplicate();
    std::vector<int> newVoxel;

    for(int i = 1; i<=nbVoxel; i++){
        if(isItInsideTheMesh(i)){
            newVoxel.push_back(i);
        }
    }

    std::vector<int> TotalVoxel;
    TotalVoxel.reserve( newVoxel.size() + activatedVoxel.size() );
    TotalVoxel.insert( TotalVoxel.end(), newVoxel.begin(), newVoxel.end() );
    TotalVoxel.insert( TotalVoxel.end(), activatedVoxel.begin(), activatedVoxel.end() );
    activatedVoxel = TotalVoxel;
}





