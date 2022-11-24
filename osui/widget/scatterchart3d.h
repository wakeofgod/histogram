#ifndef SCATTERCHART3D_H
#define SCATTERCHART3D_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>


class ScatterChart3D:public QOpenGLWidget,protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    ScatterChart3D(QWidget *parent = nullptr);
    ~ScatterChart3D();

    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void loadScatterData(QList<QVector3D> &qvector3dList);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

private:
    QOpenGLShaderProgram *mShader;
    unsigned int VBOs[4], VAOs[2];
    float aspectRatio;
    QVector3D cameraPos;

    //旋转角度
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    //缩放比例
    GLfloat scaling = 0.8f;

    //鼠标位置
    QPoint mLastPos;

    QVector<GLfloat> vboVec;
    QVector<GLfloat> vcoVec;
    QList<QVector3D> qvector3dList;
};

#endif // SCATTERCHART3D_H
