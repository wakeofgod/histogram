#ifndef WBCCHART_H
#define WBCCHART_H
#include <QWidget>
#include <QtGui>

class WBCChart: public QWidget
{
public:
    WBCChart(QWidget *parent = nullptr);
    ~WBCChart();

    void resetData(QVector<int> &statisticsWBC);

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

#endif // WBCCHART_H
