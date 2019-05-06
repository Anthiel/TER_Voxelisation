#ifndef MESHVIEWERWIDGET_H
#define MESHVIEWERWIDGET_H

#define GL_GLEXT_PROTOTYPES
#include <QDebug>
#include <QGLWidget>
#include "QMouseEvent"
#include <OpenMesh/Core/Geometry/VectorT.hh>

#ifdef __APPLE__
    #include <gl.h>
    #include <glu.h>
    #include <glext.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
    #include <GL/glut.h>
#endif


using namespace OpenMesh;

const double TRACKBALL_RADIUS = 0.6;

class MeshViewerWidget : public QGLWidget
{
    Q_OBJECT
public:
    MeshViewerWidget(QWidget* _parent=nullptr);
    MeshViewerWidget(QGLFormat& _fmt, QWidget* _parent );

    void initializeGL();
    void resizeGL(int _w, int _h);
    void paintGL();

    void update_projection_matrix();
    void view_all();
    void set_scene_pos( const OpenMesh::Vec3f& _cog, float _radius );
    bool map_to_sphere(const QPoint& _point, OpenMesh::Vec3f& _result);
    void translate( const OpenMesh::Vec3f& _trans );
    void rotate( const OpenMesh::Vec3f& _axis, float _angle );

    void reloadPOV();
    void loadMesh(GLfloat* verts, GLfloat* colors, int nVerts, GLuint* triangles, int nTriangles);
    void loadLines(GLfloat* verts, GLfloat* colors, int nVerts, GLuint* lines, int nLines, QList<QPair<float, int> > es);
    void loadPoints(GLfloat* verts, GLfloat* colors, int nVerts, GLuint* points, int nPoints, QList<QPair<float, int> > vs);

    GLuint TriDataBuffers[2];
    int triToDraw;
    GLuint LinesDataBuffers[2];
    int linesToDraw;
    QList<QPair<float, int> > edgeSizes;
    GLuint PointsDataBuffers[2];
    int pointsToDraw;
    QList<QPair<float, int> > vertsSizes;
    bool init;
    OpenMesh::Vec3f  center_;
    float            radius_;
    GLdouble    projection_matrix_[16], modelview_matrix_[16];
    QPoint           last_point_2D_;
    OpenMesh::Vec3f  last_point_3D_;
    bool             last_point_ok_;
protected:
    virtual void mousePressEvent( QMouseEvent* );
    virtual void mouseReleaseEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );
    virtual void wheelEvent( QWheelEvent* );
};

#endif // MESHVIEWERWIDGET_H
