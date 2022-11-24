#ifndef RBCCHART_H
#define RBCCHART_H
#include <QWidget>
#include <QtGui>

class RBCChart: public QWidget
{
public:
    RBCChart(QWidget *parent = nullptr);
    ~RBCChart();

    void resetData(QVector<int> &statisticsRBC);

protected:
    void paintEvent(QPaintEvent *event);
    void drawBackground(QPainter *painter);
    //绘制表格
    void drawTable(QPainter *painter);
    void drawImage(QPainter *painter);
    //绘制静态直方图
    void drawRBCStaticImage(QPainter *painter,int model=1);
private:
    QColor Background;              //背景色
    QImage m_Image;
    int m_CurrentChartOP = 0;

    int LeftMargin = 20;
    int TopMargin = 20;
    int RightMargin = 20;
    int BottomMargin = 30;

    QVector<int> data;
};

#endif // RBCCHART_H
