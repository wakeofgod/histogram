#ifndef PLTCHART_H
#define PLTCHART_H
#include <QWidget>
#include <QtGui>

class PLTChart: public QWidget
{
public:
    PLTChart(QWidget *parent = nullptr);
    ~PLTChart();

    void resetData(QVector<int> &statisticsPLT);
protected:
    void paintEvent(QPaintEvent *event);
    void drawBackground(QPainter *painter);
    //绘制表格
    void drawTable(QPainter *painter);
    void drawImage(QPainter *painter);
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

#endif // PLTCHART_H
