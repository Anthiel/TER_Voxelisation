#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <QIcon>
#include <QShortcut>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTime>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <iostream>

#include <DGtal/helpers/StdDefs.h>
#include <DGtal/base/Common.h>
#include <DGtal/shapes/Mesh.h>
#include <DGtal/shapes/MeshVoxelizer.h>
#include <DGtal/io/boards/Board3D.h>
#include <DGtal/io/writers/MeshWriter.h>


namespace Ui {
    class MainWindow;
}

using namespace OpenMesh;
using namespace OpenMesh::Attributes;


struct MyTraits : public OpenMesh::DefaultTraits {
    // use vertex normals and vertex colors
    VertexAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
    // store the previous halfedge
    HalfedgeAttributes( OpenMesh::Attributes::PrevHalfedge );
    // use face normals face colors
    FaceAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
    EdgeAttributes( OpenMesh::Attributes::Color | OpenMesh::Attributes::Status );
    // vertex thickness
    VertexTraits{float thickness; float value;};
    // edge thickness
    EdgeTraits{float thickness;};
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void voxelizeDGtal(MyMesh *_mesh);
    void voxelizePtal(MyMesh *_mesh);

    void displayMesh(MyMesh *_mesh, bool isTemperatureMap = false, float mapRange = -1);
    void resetAllColorsAndThickness(MyMesh* _mesh);
    void deleteUselessVertices(MyMesh *_mesh);

    // Widgets de l'IHM
    void showOrHideResults(bool visible);
    void showOrHideCoverTime(bool visible);
    void showOrHideVoxelTime(bool visible);
    void showOrHideFillTime(bool visible);
    void showOrHideGenerateTime(bool visible);

private slots:
    void on_action_RAW_triggered();
    void on_actionOuvrir_triggered();
    void on_actionQuitter_triggered();
    void on_action_VOL_triggered();

    void on_DGtalButton_clicked();
    void on_PtalButton_clicked();

    void on_AccuracySlider_valueChanged(int value);
    void on_AccuracyValueSpinbox_valueChanged(int arg1);

private:
    const QIcon icon_open = QIcon(":/icons/open.png");
    const QIcon icon_export = QIcon(":/icons/export.png");
    const QIcon icon_quit = QIcon(":/icons/quit.png");
    const QIcon icon_file_raw = QIcon(":/icons/file-raw.png");
    const QIcon icon_file_vol = QIcon(":/icons/file-vol.png");

    MyMesh mesh;
    QTime timer;
    QString currentFileName = "";
    QString currentBaseFileName = "";
    QString noPointsFileName = "";

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
