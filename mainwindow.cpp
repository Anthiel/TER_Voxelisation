#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Space.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    vertexSelection = -1;
    edgeSelection = -1;
    faceSelection = -1;
    modevoisinage = false;

    ui->setupUi(this);
    this->setWindowTitle("Projet TER - Voxélisation");
    showOrHideResults(false);

    this->ui->actionOuvrir->setIcon(icon_open);
    this->ui->menuExporter->setIcon(icon_export);
    this->ui->actionQuitter->setIcon(icon_quit);
    this->ui->action_RAW->setIcon(icon_file_raw);
    this->ui->action_VOL->setIcon(icon_file_vol);

    this->ui->PtalComboBox->addItem("par vertices", int(Space::VoxelisationByVertice));
    this->ui->PtalComboBox->addItem("par edges", int(Space::VoxelisationByEdge));
    this->ui->PtalComboBox->addItem("par faces", int(Space::VoxelisationByFace));

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SLOT(on_actionOuvrir_triggered()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(on_actionQuitter_triggered()));
}
MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::voxelizeDGtal(MyMesh* _mesh){

    DGtal::trace.beginBlock("Voxelizer DGtal");
    DGtal::Mesh<DGtal::Z3i::Point> aMesh;

//    this->ui->DGtalProgressBar->setValue(10);

    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++) {
        VertexHandle current = *curVert;
        OpenMesh::Vec3f point = _mesh->point(current);
        aMesh.addVertex(DGtal::Z3i::Point(point[0], point[1], point[2]));
    }

//    this->ui->DGtalProgressBar->setValue(40);

    for(MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++){
        FaceHandle current = *curFace;
        std::vector<int> verticesIndex;
        for(MyMesh::FaceVertexIter curVert = _mesh->fv_iter(current); curVert.is_valid(); curVert++){
            VertexHandle currentVertex = *curVert;
            verticesIndex.push_back(currentVertex.idx());
        }
        if(verticesIndex.size() == 3) {
            aMesh.addTriangularFace(verticesIndex[0], verticesIndex[1], verticesIndex[2]);
//            DGtal::trace.info() << "Added triangular face (" << verticesIndex[0] << " " << verticesIndex[1] << " " << verticesIndex[2] << ")" << std::endl;
        } else if(verticesIndex.size() == 4) {
            aMesh.addQuadFace(verticesIndex[0], verticesIndex[1], verticesIndex[2], verticesIndex[3]);
//            DGtal::trace.info() << "Added quad face (" << verticesIndex[0] << " " << verticesIndex[1] << " " << verticesIndex[2] << verticesIndex[3] << ")" << std::endl;
        }
    }

//    this->ui->DGtalProgressBar->setValue(60);

    DGtal::Z3i::Domain domain(DGtal::Z3i::Point(0,0,0), DGtal::Z3i::Point(128, 128, 128));
    DGtal::Z3i::DigitalSet outputSet(domain);
    DGtal::MeshVoxelizer<DGtal::Z3i::DigitalSet, 6> voxelizer;
    DGtal::trace.info() << "Digitization..." << std::endl;
    voxelizer.voxelize(outputSet, aMesh, 4.0/*static_cast<double>(this->ui->AccuracySlider->value()) scaleFactor */);
    DGtal::trace.info()<< "Got " << outputSet.size() << " voxels." << std::endl;

    this->ui->resultsVoxelsCount->setText(QString::number(outputSet.size()));
//    this->ui->DGtalProgressBar->setValue(90);

    DGtal::Board3D<> board;
    for(auto voxel : outputSet)
      board << voxel;
    std::string fileName = (currentFileName + "_voxelizedDGtal.obj").toStdString();
    board.saveOBJ(fileName);
    DGtal::trace.endBlock();

//    this->ui->DGtalProgressBar->setValue(100);

//    OpenMesh::IO::read_mesh(mesh, "voxelizedObject.obj");
//    mesh.update_normals();
//    resetAllColorsAndThickness(&mesh);
//    displayMesh(&mesh);
}

void MainWindow::voxelizePtal(MyMesh* _mesh){
    this->ui->statusBar->showMessage("Voxélisation en cours...");

    int size = this->ui->AccuracySlider->value();
    Space world(_mesh, static_cast<Space::Voxelisation>(this->ui->PtalComboBox->currentData().toInt()), size);
    world.createSpace();
    QString stillPointsFileName = currentFileName + "_voxelizedPtal_" + QString::number(size) + ".obj";
    world.voxelize(stillPointsFileName, this->ui->fillCheckbox->isChecked());
    this->ui->resultsVoxelsCount->setText(QString::number(world.getTotalVoxels()));

    //lecture du mesh de l'obj de la voxelisation pour y enlever les points inutiles
    OpenMesh::IO::read_mesh(mesh, stillPointsFileName.toUtf8().constData());
    mesh.update_normals();
    resetAllColorsAndThickness(&mesh);
    del_uselesspoints(&mesh);
    system(qPrintable("rm " + stillPointsFileName));
    system(qPrintable("mv " + noPointsFileName + " " + stillPointsFileName));

    this->ui->statusBar->showMessage("Voxélisation terminée !");

    OpenMesh::IO::read_mesh(mesh, stillPointsFileName.toUtf8().constData());
    mesh.update_normals();
    resetAllColorsAndThickness(&mesh);
    displayMesh(&mesh);
}

void MainWindow::del_uselesspoints(MyMesh *_mesh){
    //supprime les points de Space non utilisés

    std::vector<OpenMesh::Vec3f> _points_gardes;

    //premier tour des sommets du maillage pour supprimer les sommets inutiles et mise à niveau des indices
    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++) {
        VertexHandle current = *curVert;
        if(_mesh->valence(current) == 0){
            _mesh->delete_vertex(current);
        }
    }
    _mesh->garbage_collection();

    //conservation dans _points_gardes des sommets du maillage
    for(MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++){
        VertexHandle current = *curVert;
        _points_gardes.push_back(_mesh->point(current));
    }

    //conservation des faces dans _faces
    std::vector<std::vector<int>> _faces;
    for(MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++) {
        FaceHandle currentFace = *curFace;
        std::vector<int> sommets;
        for(MyMesh::FaceVertexIter curfv = _mesh->fv_iter(currentFace); curfv.is_valid(); curfv++){
            VertexHandle vert = *curfv;
            sommets.push_back(vert.idx());
        }
        _faces.push_back(sommets);
    }

    //lecture de l'obj du mesh
    std::ofstream meshObj;
    noPointsFileName = currentFileName + "test.obj";
    meshObj.open(noPointsFileName.toStdString());

    //ecriture du nouvel obj
    //écriture des coordonnées des sommets
    for(auto i : _points_gardes){
        meshObj << "v " << i << "\n";
    }

    //écriture des faces ( f sommet1 sommet2 sommet3 \n)
    //dans OpenMesh les indices commencent à 0 alors que dans l'écriture d'un obj elle commence à 1, d'où le +1
    for(auto j : _faces){
        meshObj << "f " << j[0]+1 << " " << j[1]+1 << " " << j[2]+1 << "\n";
    }
}

void MainWindow::resetAllColorsAndThickness(MyMesh* _mesh)
{
    for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
    {
        _mesh->data(*curVert).thickness = 1;
        _mesh->set_color(*curVert, MyMesh::Color(0, 0, 0));
    }

    for (MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++)
    {
        _mesh->set_color(*curFace, MyMesh::Color(150, 150, 150));
    }

    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
    {
        _mesh->data(*curEdge).thickness = 1;
        _mesh->set_color(*curEdge, MyMesh::Color(0, 0, 0));
    }
}
void MainWindow::displayMesh(MyMesh* _mesh, bool isTemperatureMap, float mapRange)
{
    GLuint* triIndiceArray = new GLuint[_mesh->n_faces() * 3];
    GLfloat* triCols = new GLfloat[_mesh->n_faces() * 3 * 3];
    GLfloat* triVerts = new GLfloat[_mesh->n_faces() * 3 * 3];

    int i = 0;

    if(isTemperatureMap)
    {
        QVector<float> values;

        if(mapRange == -1)
        {
            for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
                values.append(fabs(_mesh->data(*curVert).value));
            std::sort(values.begin(), values.end());
            mapRange = values.at(values.size()*0.8);
            qDebug() << "mapRange" << mapRange;
        }

        float range = mapRange;
        MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
        MyMesh::ConstFaceVertexIter fvIt;

        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = _mesh->cfv_iter(*fIt);
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++;
        }
    }
    else
    {
        MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
        MyMesh::ConstFaceVertexIter fvIt;
        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = _mesh->cfv_iter(*fIt);
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++;
        }
    }


    ui->displayWidget->loadMesh(triVerts, triCols, _mesh->n_faces() * 3 * 3, triIndiceArray, _mesh->n_faces() * 3);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

    GLuint* linesIndiceArray = new GLuint[_mesh->n_edges() * 2];
    GLfloat* linesCols = new GLfloat[_mesh->n_edges() * 2 * 3];
    GLfloat* linesVerts = new GLfloat[_mesh->n_edges() * 2 * 3];

    i = 0;
    QHash<float, QList<int> > edgesIDbyThickness;
    for (MyMesh::EdgeIter eit = _mesh->edges_begin(); eit != _mesh->edges_end(); ++eit)
    {
        float t = _mesh->data(*eit).thickness;
        if(t > 0)
        {
            if(!edgesIDbyThickness.contains(t))
                edgesIDbyThickness[t] = QList<int>();
            edgesIDbyThickness[t].append((*eit).idx());
        }
    }
    QHashIterator<float, QList<int> > it(edgesIDbyThickness);
    QList<QPair<float, int> > edgeSizes;
    while (it.hasNext())
    {
        it.next();

        for(int e = 0; e < it.value().size(); e++)
        {
            int eidx = it.value().at(e);

            MyMesh::VertexHandle vh1 = _mesh->to_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh1)[0];
            linesVerts[3*i+1] = _mesh->point(vh1)[1];
            linesVerts[3*i+2] = _mesh->point(vh1)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;

            MyMesh::VertexHandle vh2 = _mesh->from_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh2)[0];
            linesVerts[3*i+1] = _mesh->point(vh2)[1];
            linesVerts[3*i+2] = _mesh->point(vh2)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;
        }
        edgeSizes.append(qMakePair(it.key(), it.value().size()));
    }

    ui->displayWidget->loadLines(linesVerts, linesCols, i * 3, linesIndiceArray, i, edgeSizes);

    delete[] linesIndiceArray;
    delete[] linesCols;
    delete[] linesVerts;

    GLuint* pointsIndiceArray = new GLuint[_mesh->n_vertices()];
    GLfloat* pointsCols = new GLfloat[_mesh->n_vertices() * 3];
    GLfloat* pointsVerts = new GLfloat[_mesh->n_vertices() * 3];

    i = 0;
    QHash<float, QList<int> > vertsIDbyThickness;
    for (MyMesh::VertexIter vit = _mesh->vertices_begin(); vit != _mesh->vertices_end(); ++vit)
    {
        float t = _mesh->data(*vit).thickness;
        if(t > 0)
        {
            if(!vertsIDbyThickness.contains(t))
                vertsIDbyThickness[t] = QList<int>();
            vertsIDbyThickness[t].append((*vit).idx());
        }
    }
    QHashIterator<float, QList<int> > vitt(vertsIDbyThickness);
    QList<QPair<float, int> > vertsSizes;

    while (vitt.hasNext())
    {
        vitt.next();

        for(int v = 0; v < vitt.value().size(); v++)
        {
            int vidx = vitt.value().at(v);

            pointsVerts[3*i+0] = _mesh->point(_mesh->vertex_handle(vidx))[0];
            pointsVerts[3*i+1] = _mesh->point(_mesh->vertex_handle(vidx))[1];
            pointsVerts[3*i+2] = _mesh->point(_mesh->vertex_handle(vidx))[2];
            pointsCols[3*i+0] = _mesh->color(_mesh->vertex_handle(vidx))[0];
            pointsCols[3*i+1] = _mesh->color(_mesh->vertex_handle(vidx))[1];
            pointsCols[3*i+2] = _mesh->color(_mesh->vertex_handle(vidx))[2];
            pointsIndiceArray[i] = i;
            i++;
        }
        vertsSizes.append(qMakePair(vitt.key(), vitt.value().size()));
    }

    ui->displayWidget->loadPoints(pointsVerts, pointsCols, i * 3, pointsIndiceArray, i, vertsSizes);

    delete[] pointsIndiceArray;
    delete[] pointsCols;
    delete[] pointsVerts;
}

void MainWindow::on_action_RAW_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en .raw", QDir::currentPath(), tr("RAW (*.raw)"));
    qDebug() << fileName;
    if(fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Erreur"), QString("Vous devez spécifier un nom de fichier!"));
        return;
    }

    fileName += ".raw";

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        for(MyMesh::FaceIter curFace = mesh.faces_begin(); curFace != mesh.faces_end(); curFace++){
            FaceHandle current = *curFace;
            for(MyMesh::FaceVertexIter curVert = mesh.fv_iter(current); curVert.is_valid(); curVert++){
                VertexHandle currentVertex = *curVert;
                OpenMesh::Vec3f point = mesh.point(currentVertex);
                stream << point[0] << " " << point[1] << " " << point[2] << " ";
            }
            stream << "\n";
        }
        file.close();
    }
    this->ui->statusBar->showMessage("Exportation en \".RAW\" réussie !");
}
void MainWindow::on_action_VOL_triggered()
{
    QString path = QDir::currentPath();

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en .vol", QDir::currentPath(), tr("VOL (*.vol)"));
    if(fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Erreur"), QString("Vous devez spécifier un nom de fichier!"));
        return;
    }

    fileName += ".vol";
    QString tempFileName = fileName;
    tempFileName.remove(".vol") += ".off";
    OpenMesh::IO::write_mesh(mesh, tempFileName.toUtf8().constData());

    system(qPrintable("mesh2vol -i " + tempFileName + " --resolution 256 -o " + fileName));
    system(qPrintable("rm " + tempFileName));
    this->ui->statusBar->showMessage("Exportation en \".VOL\" réussie !");
}

void MainWindow::on_actionOuvrir_triggered()
{
    // fenêtre de sélection des fichiers
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh"), "", tr("Mesh Files (*.obj)"));

    QFileInfo fileInfo = QFileInfo(fileName);
    currentFileName = fileInfo.baseName();
    // chargement du fichier .obj dans la variable globale "mesh"
    OpenMesh::IO::read_mesh(mesh, fileName.toUtf8().constData());

    mesh.update_normals();

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);
    this->ui->DGtalButton->setEnabled(true);
//    this->ui->DGtalProgressBar->setValue(0);

    this->ui->PtalButton->setEnabled(true);
//    this->ui->PtalProgressBar->setValue(0);
    showOrHideResults(false);
    // on affiche le maillage
    displayMesh(&mesh);
}
void MainWindow::on_actionQuitter_triggered()
{
    QApplication::quit();
}

void MainWindow::on_DGtalButton_clicked()
{
//    this->ui->DGtalProgressBar->setValue(0);
//    this->ui->DGtalProgressBar->setVisible(true);
    timer.start();
    this->voxelizeDGtal(&mesh);
    int mseconds = timer.elapsed();
    this->ui->resultsTimeCount->setText(QString::number(mseconds));
    showOrHideResults(true);
}
void MainWindow::on_PtalButton_clicked()
{
//    this->ui->PtalProgressBar->setValue(0);
//    this->ui->PtalProgressBar->setVisible(true);
    timer.start();
    this->voxelizePtal(&mesh);
    int mseconds = timer.elapsed();
    this->ui->resultsTimeCount->setText(QString::number(mseconds));
    showOrHideResults(true);
}

void MainWindow::on_AccuracySlider_valueChanged(int value)
{
    this->ui->AccuracyValueSpinbox->setValue(value);
}

void MainWindow::on_AccuracyValueSpinbox_valueChanged(int arg1)
{
    this->ui->AccuracySlider->setValue(arg1);
}

void MainWindow::showOrHideResults(bool visible){
//    this->ui->DGtalProgressBar->setVisible(visible);
//    this->ui->PtalProgressBar->setVisible(visible);
    this->ui->resultsLabel->setVisible(visible);
    this->ui->resultsTimeCount->setVisible(visible);
    this->ui->resultsTimeLabel->setVisible(visible);
    this->ui->resultsVoxelsCount->setVisible(visible);
    this->ui->resultsVoxelsLabel->setVisible(visible);
}
