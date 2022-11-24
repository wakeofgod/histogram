#include "pltchart.h"

PLTChart::PLTChart(QWidget *parent):QWidget(parent)
{
    Background = QColor(10, 10, 10);
}

PLTChart::~PLTChart()
{

}

void PLTChart::resetData(QVector<int> &statisticsPLT)
{
    this->data = statisticsPLT;
    update();
}

void PLTChart::paintEvent(QPaintEvent *event)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景
    drawBackground(&painter);
    drawTable(&painter);
    if(this->data.count() > 0)
    {
        //绘制直方图
        double tmpBuf[1024];
        for(int i = 0;i< 1024;i++)
        {
            tmpBuf[i] = this->data[i];
        }
        //20点平滑5次
        int j;
        for(int k = 1;k<=5;k++)
        {
            for(int i = 0;i< 1024;i++)
            {
                for(j=1; j<20;j++)
                {
                    if(i+j>=1024) break;
                    tmpBuf[i] += tmpBuf[i+j];
                }
                tmpBuf[i]/=j;
            }
        }
        double bmpBuf[1024];
        memset(bmpBuf,0,sizeof(bmpBuf));
        //1024->128 1024压缩至128个通道
        double a1 = 1.0f;
        for(int i = 0;i< 1024/8;i++)
        {
            bmpBuf[i] =  tmpBuf[i*8+0];
            bmpBuf[i] += tmpBuf[i*8+1];
            bmpBuf[i] += tmpBuf[i*8+2];
            bmpBuf[i] += tmpBuf[i*8+3];
            bmpBuf[i] += tmpBuf[i*8+4];
            bmpBuf[i] += tmpBuf[i*8+5];
            bmpBuf[i] += tmpBuf[i*8+6];
            bmpBuf[i] += tmpBuf[i*8+7];
            bmpBuf[i] /= 8;
            if(a1 < bmpBuf[i]){ a1 = bmpBuf[i] ;}
        }

        //查找图像数据最大值和最小值
        double value_max =0;
        double value_min = 10000;
        for(int i = 0;i< 1024/8;i++)
        {
            double e = bmpBuf[i];
            if(e>value_max)
            {
                value_max = e;
            }
            if(e<value_min)
            {
                value_min = e;
            }
        }

        //图像修正因子
        float f1 = 1.0f;
        int validH = 300;
        if(0 != value_max)
        {
            f1 = (float)(validH*0.60f)/(float)value_max;
        }

        QList<QPointF> points;
        //计算开始界标
        int startMark=0;
        //计算末端界标
        int endMark=1024/8;

        float dotX = 300.0/(1024/8);//300fL对应1024/8个通道
        float x = 0;//偏移量
        float y = 0;

        for(int i=startMark;i<endMark;i++)
        {
            x = dotX*i;

            y = bmpBuf[i]*f1;

            float x1 = (float)(20*x/50.0)+LeftMargin;//20为前偏移通道

            float y1 = (this->height()-BottomMargin)-y-1;

            points.append(QPointF(x1,y1));
        }

        //绘制直方图
        painter.save();
        QPen pen2;
        pen2.setWidth(1);
        pen2.setColor(QColor(0,255,0));//直方图颜色
        painter.setPen(pen2);
        for(int i=0;i<points.count()-1;i++)
        {
            QPointF p1 = QPointF(points[i].x(),points[i].y());
            QPointF p2 = QPointF(points[i+1].x(),points[i+1].y());
            painter.drawLine(p1,p2);
        }
        painter.restore();
    }

}

void PLTChart::drawBackground(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(Background);
    painter->drawRoundedRect(rect(), 0, 0);

    painter->restore();
}

void PLTChart::drawTable(QPainter *painter)
{
    int image_width = this->width();
    int image_height = this->height();

    painter->save();
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0x00,0xBB,0x9E));
    painter->setPen(pen);

    //垂直轴
    painter->drawLine(QPointF(this->LeftMargin,this->TopMargin),QPointF(this->LeftMargin,image_height-this->BottomMargin));
    painter->save();
    painter->setPen(QColor(0x00,0xBB,0x9E));//00BB9E
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    painter->setFont(font);
    QFontMetrics fm1(font);
    QString label = "PLT";
    painter->drawText(QPointF(image_width-this->RightMargin-fm1.boundingRect(label).width(),this->TopMargin), label);
    painter->restore();
    //水平轴
    painter->drawLine(QPointF(this->LeftMargin,image_height-this->BottomMargin),QPointF(image_width-this->RightMargin,image_height-this->BottomMargin));
    //水平刻度
    int mark_width = (image_width-this->LeftMargin-this->RightMargin)/6;
    QFont font1;
    font1.setPointSize(11);
    painter->setFont(font1);
    QFontMetrics fm(font1);//测量字符象素
    for(int i=0;i<6;i++)
    {
        //画刻度
        painter->drawLine(QPointF(this->LeftMargin+mark_width*(i+1),image_height-this->BottomMargin+2),QPointF(this->LeftMargin+mark_width*(i+1),image_height-this->BottomMargin+5));
        painter->save();//保存前一个状态
        if(i%2==0 && i!=5)
        {
            painter->restore();
            continue;
        }
        painter->setPen(QColor(0x00,0xBB,0x9E));
        QString xmarkstr;
        if(i==5)
        {
            xmarkstr = QString("fL");
        }
        else
        {
            xmarkstr = QString("%1").arg(50*(i+1));
        }

        QRect rec = fm.boundingRect(xmarkstr);
        painter->drawText(QRect(this->LeftMargin+mark_width*(i+1)-rec.width()/2,image_height-this->BottomMargin+5,rec.width(),rec.height()), Qt::AlignCenter,xmarkstr);
        painter->restore();//恢复
    }

    painter->restore();
}

void PLTChart::drawImage(QPainter *painter)
{

}
