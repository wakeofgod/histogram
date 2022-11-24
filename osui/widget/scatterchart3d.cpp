#include "scatterchart3d.h"

const int TOTAL = 2000;
static const char* VERTEX_SHADER_CODE =
        "#version 330 \n"
        "layout(location = 0) in vec3 posVertex;\n"
        "layout(location = 1) in vec3 colVertex;\n"
        "uniform mat4 MVP;\n"
        "out vec4 cols;\n"
        "void main() {\n"
        "  gl_Position = MVP * vec4(posVertex, 1.0f);\n"
        "  gl_PointSize = 2.0f;\n"
        "  cols = vec4(colVertex, 1.0f);\n"
        "}\n";

static const char* FRAGMENT_SHADER_CODE =
        "#version 330\n"
        "in vec4 cols;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "  fragColor = cols;\n"
        "}\n";
ScatterChart3D::ScatterChart3D(QWidget *parent):
    QOpenGLWidget(parent),
    cameraPos(0.0f,3.0f,0.0f)
{

}

ScatterChart3D::~ScatterChart3D()
{

}

void ScatterChart3D::initializeGL()
{
    this->initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    mShader = new QOpenGLShaderProgram();
    mShader->addShaderFromSourceCode(QOpenGLShader::Vertex,VERTEX_SHADER_CODE);
    mShader->addShaderFromSourceCode(QOpenGLShader::Fragment,FRAGMENT_SHADER_CODE);
    bool success = mShader->link();
    if(!success)
    {
        qDebug()<<QString("shader link failed");
    }

/*
    GLfloat vertices[] = {
        //正面正方形
        +1.0, +1.0, +1.0,
        -1.0, +1.0, +1.0,
        -1.0, -1.0, +1.0,
        +1.0, -1.0, +1.0,
        //背面正方形
        +1.0, +1.0, -1.0,
        -1.0, +1.0, -1.0,
        -1.0, -1.0, -1.0,
        +1.0, -1.0, -1.0,
        //4条边
        +1.0, +1.0, +1.0,
        +1.0, +1.0, -1.0,
        //
        -1.0, +1.0, +1.0,
        -1.0, +1.0, -1.0,
        //
        -1.0, -1.0, +1.0,
        -1.0, -1.0, -1.0,
        //
        +1.0, -1.0, +1.0,
        +1.0, -1.0, -1.0,
        //点
        +0.5, +0.5, +0.5,
        +0.5, +0.75, +0.75,
    };
*/

    /*
    GLfloat vertices[] = {
        //正面正方形
        +1.0, +1.0, +1.0,
        -1.0, +1.0, +1.0,
        -1.0, -1.0, +1.0,
        +1.0, -1.0, +1.0,
        //背面正方形
        +1.0, +1.0, -1.0,
        -1.0, +1.0, -1.0,
        -1.0, -1.0, -1.0,
        +1.0, -1.0, -1.0,
        //4条边
        +1.0, +1.0, +1.0,
        +1.0, +1.0, -1.0,
        //
        -1.0, +1.0, +1.0,
        -1.0, +1.0, -1.0,
        //
        -1.0, -1.0, +1.0,
        -1.0, -1.0, -1.0,
        //
        +1.0, -1.0, +1.0,
        +1.0, -1.0, -1.0,

        +1.0, +1.0, +1.0,
        +1.0, -1.0, +1.0,

        +1.0, +1.0, -1.0,
        +1.0, -1.0, -1.0,
    };
*/
    GLfloat vertices[] = {
        //正面正方形
        +1.0, +1.0, +1.0,
        -1.0, +1.0, +1.0,
        -1.0, -1.0, +1.0,
        +1.0, -1.0, +1.0,
        //背面正方形,重复的点是为了纯色
        +1.0, +1.0, -1.0,
        -1.0, +1.0, -1.0,
        +1.0, +1.0, -1.0,
        -1.0, +1.0, -1.0,
        -1.0, -1.0, -1.0,
        +1.0, -1.0, -1.0,
        //4条边
        +1.0, +1.0, +1.0,
        +1.0, +1.0, -1.0,
        //重复的点
        -1.0, +1.0, +1.0,
        -1.0, +1.0, -1.0,
        -1.0, +1.0, +1.0,
        -1.0, +1.0, -1.0,
        //
        -1.0, -1.0, +1.0,
        -1.0, -1.0, -1.0,
        //
        +1.0, -1.0, +1.0,
        +1.0, -1.0, -1.0,

        +1.0, +1.0, +1.0,
        +1.0, -1.0, +1.0,

        +1.0, +1.0, -1.0,
        +1.0, -1.0, -1.0,
    };

    /*
    float cuboColor[] = {
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        1.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,1.0f
    };
    */
    float cuboColor[] = {
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,

        1.0f,0.0f,0.0f,
        1.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,

        0.0f,0.0f,1.0f,
        0.0f,0.0f,1.0f,
    };

    glGenVertexArrays(2,VAOs);
    glGenBuffers(3,VBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER,VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cuboColor), cuboColor, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAOs[1]);	// note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);	// and a different VBO
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * TOTAL*3,this->vboVec.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    //this->vcoVec.clear();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,VBOs[3]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * TOTAL*3,this->vcoVec.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
}

void ScatterChart3D::resizeGL(int w, int h)
{
    aspectRatio= (float)w/h;
}

void ScatterChart3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
    mShader->bind();
    QMatrix4x4 mvp;
    mvp.perspective(45.0f,this->aspectRatio,0.1f,100.0f);
    QVector3D center(cameraPos);
    center.setY(0);
    mvp.lookAt(cameraPos,center,QVector3D(1.0f,0.0f,0.0f));
    //mvp.setToIdentity();

    //旋转
    mvp.rotate(this->m_xRot/16, 1,0,0);
    mvp.rotate(this->m_yRot/16, 0,1,0);
    mvp.rotate(this->m_zRot/16, 0,0,1);

    //缩放
    mvp.scale(this->scaling);

    mShader->setUniformValue(mShader->uniformLocation("MVP"),mvp);

    glBindVertexArray(VAOs[0]);
    /*
       glDrawArrays(GL_LINE_LOOP, 0, 4);
       glDrawArrays(GL_LINE_LOOP, 4, 4);
       glDrawArrays(GL_LINES, 8, 2);
       glDrawArrays(GL_LINES, 10, 2);
       glDrawArrays(GL_LINES, 12, 2);
       glDrawArrays(GL_LINES, 14, 2);
    */

    /*
       glDrawArrays(GL_LINES, 0, 2);
       glDrawArrays(GL_LINES, 1, 2);
       glDrawArrays(GL_LINES, 2, 2);

       glDrawArrays(GL_LINES, 4, 2);
       glDrawArrays(GL_LINES, 5, 2);
       glDrawArrays(GL_LINES, 6, 2);

       glDrawArrays(GL_LINES, 8, 2);
       glDrawArrays(GL_LINES, 10, 2);
       glDrawArrays(GL_LINES, 12, 2);
       glDrawArrays(GL_LINES, 14, 2);
       glDrawArrays(GL_LINES, 16, 2);
       glDrawArrays(GL_LINES, 18, 2);
    */

    glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_LINES, 1, 2);
    glDrawArrays(GL_LINES, 2, 2);

    glDrawArrays(GL_LINES, 4, 2);
    glDrawArrays(GL_LINES, 7, 2);
    glDrawArrays(GL_LINES, 8, 2);

    glDrawArrays(GL_LINES, 10, 2);
    glDrawArrays(GL_LINES, 12, 2);
    glDrawArrays(GL_LINES, 16, 2);
    glDrawArrays(GL_LINES, 18, 2);
    glDrawArrays(GL_LINES, 20, 2);
    glDrawArrays(GL_LINES, 22, 2);

    glBindVertexArray(VAOs[1]);
    if(this->vcoVec.count()>0)
    {
        glDrawArrays(GL_POINTS,0,this->vcoVec.count());
    }
}

void ScatterChart3D::loadScatterData(QList<QVector3D> &source)
{
    this->qvector3dList.clear();
    this->vboVec.clear();
    this->vcoVec.clear();
    this->qvector3dList = source;
    int totalNum = this->qvector3dList.count();
    //qDebug()<<QString("ScatterChart3D::loadScatterData %1").arg(totalNum);
 //随机数
#if 0
    for (int i = 0; i < totalNum; ++i)
    {
        QVector3D d;
        int posX = QRandomGenerator::global()->bounded(-100,100);
        int posY = QRandomGenerator::global()->bounded(-100,100);
        int posZ = QRandomGenerator::global()->bounded(-100,100);
        d.setX(posX/100.0f);
        d.setY(posY/100.0f);
        d.setZ(posZ/100.0f);
        this->qvector3dList.append(d);
        //qDebug()<<d.x()<<" "<<d.y()<<" "<<d.z();
        vboVec<< (GLfloat)posX/100.0f<<(GLfloat)posY/100.0f<<(GLfloat)posZ/100.0f;

        QVector3D color;
        int colorX= QRandomGenerator::global()->bounded(1,255);
        int colorY= QRandomGenerator::global()->bounded(1,255);
        int colorZ= QRandomGenerator::global()->bounded(1,255);

        color.setX(colorX/255.0f);
        color.setY(colorY/255.0f);
        color.setZ(colorZ/255.0f);
        vcoVec<<(GLfloat)colorX/255.0f<<(GLfloat)colorY/255.0f<<(GLfloat)colorZ/255.0f;
    }
#endif
    //解析文件的数据
#if  1
    for (int i = 0; i < totalNum; ++i)
    {

        vboVec<< source.at(i).x()<<source.at(i).y()<<source.at(i).z();

        QVector3D color;
        int colorX= QRandomGenerator::global()->bounded(1,255);
        int colorY= QRandomGenerator::global()->bounded(1,255);
        int colorZ= QRandomGenerator::global()->bounded(1,255);

        color.setX(colorX/255.0f);
        color.setY(colorY/255.0f);
        color.setZ(colorZ/255.0f);
        vcoVec<<(GLfloat)colorX/255.0f<<(GLfloat)colorY/255.0f<<(GLfloat)colorZ/255.0f;
    }
    //qDebug()<<vboVec.count();
#endif

    glBindVertexArray(VAOs[1]);	// note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);	// and a different VBO
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * totalNum*3,this->vboVec.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,VBOs[3]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * totalNum*3,this->vcoVec.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glEnableVertexAttribArray(1);
    update();
}

void ScatterChart3D::mousePressEvent(QMouseEvent *event)
{
    mLastPos = event->pos();
}

void ScatterChart3D::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - mLastPos.x();
    int dy = event->y() - mLastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    mLastPos = event->pos();
}

void ScatterChart3D::wheelEvent(QWheelEvent *event)
{
    double numDegrees = -event->delta() / 8.0;
    double numSteps = numDegrees / 15.0;
    scaling *= std::pow(1.02, numSteps);
    update();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void ScatterChart3D::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void ScatterChart3D::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void ScatterChart3D::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}
