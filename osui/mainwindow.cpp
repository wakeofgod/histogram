#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xlsxdocument.h"
#include "xlsxchart.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scatter = new ScatterChart3D(this);
    ui->horizontalLayout_3d->addWidget(scatter);

    connect(ui->xSlider, &QSlider::valueChanged, scatter, &ScatterChart3D::setXRotation);
    connect(scatter, &ScatterChart3D::xRotationChanged, ui->xSlider, &QSlider::setValue);
    connect(ui->ySlider, &QSlider::valueChanged, scatter, &ScatterChart3D::setYRotation);
    connect(scatter, &ScatterChart3D::yRotationChanged, ui->ySlider, &QSlider::setValue);
    connect(ui->zSlider, &QSlider::valueChanged, scatter, &ScatterChart3D::setZRotation);
    connect(scatter, &ScatterChart3D::zRotationChanged, ui->zSlider, &QSlider::setValue);

    connect(FrmLoadFile::Instance(),SIGNAL(signalSelectedFile(QString)),this,SLOT(slotSelectedFile(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parseFile()
{
    this->zeroSourceList.clear();
    this->tenSourceList.clear();
    this->ninetySourceList.clear();

    this->zeroCrestList.clear();
    this->tenCrestList.clear();
    this->ninetyCrestList.clear();

    this->zeroVallyList.clear();
    this->tenVallyList.clear();
    this->ninetyVallyList.clear();
    //波长
    this->zeroWaveList.clear();
    this->tenWaveList.clear();
    this->ninetyWaveList.clear();

    //qDebug()<<QString("%1").arg(this->excelPath);
    this->file = new QFile(excelPath);
    if(!file->exists()||!file->open(QIODevice::ReadOnly))
    {
        qDebug()<<QString("file not exists");
    }

    QByteArray array = file->readAll();
    if(array.count()<= 0)
    {
        return;
    }

    for (int i = 0; i < array.count(); ++i)
    {
        //查找tek包头,一个包320个字节，6个头，240个有效数据，其他忽略
        if(array.at(i) == 0x54 && array.at(i+1) == 0x45 && array.at(i+2) == 0x4b)
        {
            int position = i +6;
            for (int j = position; j < position + 240 && j < array.count(); j+=6)
            {
                //取编号
                //uchar zeroIndex = array.at(j+1)>>4;
                uchar zeroRight = array.at(j+1) & 0x0f;
                quint16 zeroValue = (quint16)(zeroRight<<8) + (quint8)(array.at(j));

                //uchar tenIndex = array.at(j+3)>>4;
                uchar tenRight = array.at(j+3) &0x0f;
                quint16 tenValue = (quint16)(tenRight<<8) + (quint8)(array.at(j+2));

                //uchar ninetyIndex = array.at(j+5)>>4;
                uchar ninetyRight = array.at(j+5) &0x0f;
                quint16 ninetyValue = (quint16)(ninetyRight<<8) + (quint8)(array.at(j+4));
                if(zeroValue > zeroMax)
                {
                    zeroMax = zeroValue;
                }
                if(tenValue > tenMax)
                {
                    tenMax = tenValue;
                }
                if(ninetyValue > ninetyMax)
                {
                    ninetyMax = ninetyValue;
                }
                //阈值
                if(zeroValue< 50 &&tenValue< 50 &&ninetyValue < 50)
                {
                    zeroSourceList.push_back(0);
                    tenSourceList.push_back(0);
                    ninetySourceList.push_back(0);
                }
                else if(zeroValue ==0)
                {
                    zeroSourceList.push_back(0);
                    tenSourceList.push_back(0);
                    ninetySourceList.push_back(0);
                }
                else
                {
                    zeroSourceList.push_back(zeroValue);
                    tenSourceList.push_back(tenValue);
                    ninetySourceList.push_back(ninetyValue);
                }
            }
        }
    }
    //qDebug()<<QString(" zeroMax=%1  tenMax=%2 ninetyMax=%3").arg(zeroMax).arg(tenMax).arg(ninetyMax);
    //气泡处理
    int iStart = 0,iEnd = 0;
    for (int i = 1; i < zeroSourceList.count()-1; i++)
    {
        if(zeroSourceList.at(i) == 0 &&zeroSourceList.at(i+1) >0)
        {
            iStart = i;
            iEnd = i;
            //qDebug()<<QString("istart = %1 iend= %2").arg(iStart).arg(iEnd);
        }
        if(zeroSourceList.at(i) >0 && zeroSourceList.at(i+1) ==0)
        {
            iEnd = i;
            //qDebug()<<QString("istart = %1 iend= %2").arg(iStart).arg(iEnd);
        }
        if((iEnd- iStart) > 25)
        {
            //qDebug()<<QString("MainWindow::on_btnStart_clicked find dirty data start= %1 end= %2").arg(iStart).arg(iEnd);
            for (int j = iStart; j <= iEnd; j++)
            {
                zeroSourceList[j] = 0;
                tenSourceList[j] = 0;
                ninetySourceList[j] = 0;
            }
            iStart = i;
            iEnd = i;
        }
    }

    iStart = 0,iEnd = 0;
    //计算波谷波长
    for (int i = 1; i < zeroSourceList.count()-9; ++i)
    {
        if(zeroSourceList.at(i) == 0 &&zeroSourceList.at(i+1) >0)
        {
            iStart = i;
        }
        if(zeroSourceList.at(i) >0 && zeroSourceList.at(i+1) ==0)
        {
            iEnd = i+1;
        }
        if(iEnd > iStart)
        {
            //qDebug()<<QString("istart = %1 iend= %2").arg(iStart+1).arg(iEnd+1);
            //开始计算
            QVector<int> tempSource;
            QVector<int> tempCrestList;
            QVector<int> tempVallyList;

            QVector<int> tempSourceTen;
            QVector<int> tempCrestListTen;
            QVector<int> tempVallyListTen;

            QVector<int> tempSourceNinety;
            QVector<int> tempCrestListNinety;
            QVector<int> tempVallyListNinety;
            //qCopy(zeroSourceList.begin() + iStart,zeroSourceList.begin() + iEnd,tempSource.begin());
            for (int j = iStart; j <= iEnd; j++)
            {
                tempSource.push_back(zeroSourceList.at(j));
                tempSourceTen.push_back(tenSourceList.at(j));
                tempSourceNinety.push_back(ninetySourceList.at(j));
            }
            findCrestList(iStart,tempSource,tempCrestList,tempVallyList);
            findCrestList(iStart,tempSourceTen,tempCrestListTen,tempVallyListTen);
            findCrestList(iStart,tempSourceNinety,tempCrestListNinety,tempVallyListNinety);
            //qDebug()<<QString("tempCrestList =%1 tempVallyList=%2").arg(tempCrestList.count()).arg(tempVallyList.count());
            //0
#if 1
            if(tempCrestList.count()>0)
            {
                for (int k = 0; k < tempCrestList.count(); ++k)
                {
                    zeroCrestList.push_back(tempCrestList.at(k));
                }
            }
            else
            {
                qDebug()<<QString("0°没有波形，强行插入一条数据");
                zeroCrestList.push_back(0);
            }
            if(tempVallyList.count()>0)
            {
                zeroVallyList.push_back(tempVallyList.at(0));
//                zeroWaveList.push_back(tempVallyList.at(0)-iStart);
//                zeroWaveList.push_back(iEnd-tempVallyList.at(0));
                //双波的波长需要拆分
                zeroWaveList.push_back((tempCrestList.at(0)-iStart)*2);
                zeroWaveList.push_back((iEnd-tempCrestList.at(1))*2);
            }
            else
            {
                zeroVallyList.push_back(iStart);
                zeroVallyList.push_back(iEnd);
                zeroWaveList.push_back(iEnd - iStart);
            }
#endif

            //10
#if 1
            if(tempCrestList.count() < tempCrestListTen.count())
            {
                int tempCrest = tempCrestListTen.at(0);
                //qDebug()<<QString("0°单波=%1，10°双波=%2 position= %3").arg(tempCrestList.count()).arg(tempCrestListTen.count()).arg(tempCrest+1);
                tenSourceList[tempCrest] = 0;
                tenCrestList.push_back(tempCrestListTen.last());
            }
            else if(tempCrestList.count() >tempCrestListTen.count() && tempCrestListTen.count()!=0)
            {
                //qDebug()<<QString("0°双波=%1，10°单波=%2").arg(tempCrestList.count()).arg(tempCrestListTen.count());;
                int tempCrest = tempCrestListTen.at(0);
                tempCrestListTen = tempCrestList;
                for (int k = 0; k < tempCrestList.count(); k++)
                {
                    tenSourceList[tempCrestList.at(k)] = tenSourceList.at(tempCrest) * 0.8;
                    tenCrestList.push_back(tempCrestList.at(k));
                }
            }
            else if(tempCrestList.count() >tempCrestListTen.count() && tempCrestListTen.count()==0)
            {
                qDebug()<<QString("10°没有波形，强行插入一条数据");
                tenCrestList.push_back(tempCrestList.at(0));
            }
            else if(tempCrestList.count()== tempCrestListTen.count() && tempCrestListTen.count()>0)
            {
                for (int k = 0; k < tempCrestListTen.count(); ++k)
                {
                    tenCrestList.push_back(tempCrestListTen.at(k));
                }
            }
            if(tempVallyList.count()> tempVallyListTen.count())
            {
                tenVallyList.push_back(tempVallyList.at(0));
                tenWaveList.push_back(tempVallyList.at(0)-iStart);
                tenWaveList.push_back(iEnd-tempVallyList.at(0));
            }
            else  if(tempVallyList.count()==tempVallyListTen.count() && tempVallyListTen.count() > 0)
            {
                tenVallyList.push_back(tempVallyListTen.at(0));
//                tenWaveList.push_back(tempVallyListTen.at(0)-iStart);
//                tenWaveList.push_back(iEnd-tempVallyListTen.at(0));
                //双波的波长需要拆分
                tenWaveList.push_back((tempCrestListTen.at(0)-iStart)*2);
                tenWaveList.push_back((iEnd-tempCrestListTen.at(1))*2);
            }
            else
            {
                tenVallyList.push_back(iStart);
                tenVallyList.push_back(iEnd);
                tenWaveList.push_back(iEnd - iStart);
            }
#endif
            //90
#if 1
            if(tempCrestList.count() < tempCrestListNinety.count())
            {
                //qDebug()<<QString("0°单波，90°双波 %1").arg(tempCrestListNinety.at(0));
                int tempCrest = tempCrestListNinety.at(0);
                ninetySourceList[tempCrest] = 0;
                ninetyCrestList.push_back(tempCrestListNinety.last());
            }
            if(tempCrestList.count() >tempCrestListNinety.count() && tempCrestListNinety.count()!=0)
            {
                //qDebug()<<QString("0°双波，90°单波 %1").arg(tempCrestListNinety.at(0)+1);
                int tempCrest = tempCrestListNinety.at(0);
                tempCrestListNinety = tempCrestList;
                for (int k = 0; k < tempCrestList.count(); k++)
                {
                    ninetySourceList[tempCrestList.at(k)] = ninetySourceList.at(tempCrest) * 0.85;
                    ninetyCrestList.push_back(tempCrestList.at(k));
                }
            }
            else if(tempCrestList.count() >tempCrestListNinety.count() && tempCrestListNinety.count()==0)
            {
                //qDebug()<<QString("90°没有波形，强行插入一条数据 %1").arg(tempCrestList.at(0));
                ninetyCrestList.push_back(tempCrestList.at(0));
            }
            else if(tempCrestList.count()== tempCrestListNinety.count() && tempCrestListNinety.count()>0)
            {
                for (int k = 0; k < tempCrestListNinety.count(); ++k)
                {
                    ninetyCrestList.push_back(tempCrestListNinety.at(k));
                }
            }
            if(tempVallyList.count() < tempVallyListNinety.count() && tempVallyList.count() == 0)
            {
                ninetyVallyList.push_back(iStart);
                ninetyVallyList.push_back(iEnd);

                ninetyWaveList.push_back(iEnd - iStart);
            }
            else if(tempVallyList.count() < tempVallyListNinety.count() && tempVallyList.count() != 0)
            {
                ninetyVallyList.push_back(tempVallyList.at(0));
                ninetyWaveList.push_back(tempVallyList.at(0)-iStart);
                ninetyWaveList.push_back(iEnd-tempVallyList.at(0));
            }
            else  if(tempVallyList.count()> tempVallyListNinety.count())
            {
                //qDebug()<<QString("111");
                ninetyVallyList.push_back(tempVallyList.at(0));
                ninetyWaveList.push_back(tempVallyList.at(0)-iStart);
                ninetyWaveList.push_back(iEnd-tempVallyList.at(0));
            }
            else if(tempVallyList.count() == tempVallyListNinety.count() && tempVallyListNinety.count()>0 && ninetySourceList.at(tempVallyListNinety.at(0))!=0)
            {
                //int tempVally = tempVallyListNinety.at(0);
                //qDebug()<<QString("position = %1").arg(tempVally);
                ninetyVallyList.push_back(tempVallyListNinety.at(0));
//                ninetyWaveList.push_back(tempVallyListNinety.at(0)-iStart);
//                ninetyWaveList.push_back(iEnd-tempVallyListNinety.at(0));
                //双波的波长需要拆分
                ninetyWaveList.push_back((tempCrestListNinety.at(0)-iStart)*2);
                ninetyWaveList.push_back((iEnd-tempCrestListNinety.at(1))*2);

            }
            else if(tempVallyList.count() == tempVallyListNinety.count()&& tempVallyListNinety.count()>0 && ninetySourceList.at(tempVallyListNinety.at(0))==0)
            {
                //qDebug()<<"444";
                ninetyVallyList.push_back(iStart);
                ninetyVallyList.push_back(tempVallyListNinety.at(0));

                ninetyWaveList.push_back(tempVallyListNinety.at(0) - iStart);
            }
            else if(tempCrestListNinety.count()==0)
            {
                ninetyVallyList.push_back(iStart);
                ninetyVallyList.push_back(iEnd);

                ninetyWaveList.push_back(0);
            }
            else
            {
                ninetyVallyList.push_back(iStart);
                ninetyVallyList.push_back(iEnd);

                ninetyWaveList.push_back(iEnd - iStart);
            }
#endif
            //处理完清空
            iStart = 0;
            iEnd = 0;
        }
    }

    QList<QVector3D> qvector3dList;
    QXlsx::Document xlsx;
    int totalCount = zeroCrestList.count();
    for (int i = 0; i < totalCount; ++i)
    {
        int posX = zeroSourceList.at(zeroCrestList.at(i));
        int posY = tenSourceList.at(tenCrestList.at(i));
        int posZ = ninetySourceList.at(ninetyCrestList.at(i));
        xlsx.write(i+1,1,posX);
        xlsx.write(i+1,2,posY);
        xlsx.write(i+1,3,posZ);
        xlsx.write(i+1,7,posX*1.0f/zeroMax);
        xlsx.write(i+1,8,posY*1.0f/tenMax);
        xlsx.write(i+1,9,posZ*1.0f/ninetyMax);
        if(posX >= 1024)
        {
            posX = 1024;
        }
        if(posY >= 1024)
        {
            posY = 1024;
        }
        if(posZ >= 1024)
        {
            posZ = 1024;
        }
        QVector3D d;
//        d.setX(posX*1.0f/zeroMax);
//        d.setY(posY*1.0f/tenMax);
//        d.setZ(posZ*1.0f/ninetyMax);

        d.setX((posX*2.0f-zeroMax)/zeroMax);
        d.setY((posY*2.0f-tenMax)/tenMax);
        d.setZ((posZ*2.0f-ninetyMax)/ninetyMax);
        qvector3dList.append(d);
    }
    xlsx.saveAs("source.xlsx");
    scatter->loadScatterData(qvector3dList);
    qApp->processEvents();
    this->statisticsWBC = QVector<int>(1024);
    this->statisticsRBC = QVector<int>(1024);
    this->statisticsPLT = QVector<int>(1024);
    for (int i = 0; i < totalCount; ++i)
    {
        for (int j = 0; j < totalCount; ++j)
        {
            int posWBC = zeroSourceList.at(zeroCrestList.at(j));
            int posRBC = tenSourceList.at(tenCrestList.at(j));
            int posPLT = ninetySourceList.at(ninetyCrestList.at(j));
            if(posWBC == i)
            {
                statisticsWBC[i]++;
            }
            if(posRBC == i)
            {
                statisticsRBC[i]++;
            }
            if(posPLT == i)
            {
                statisticsPLT[i]++;
            }
        }
    }
    for (int i = 0; i < 1024; ++i)
    {
        qDebug()<<QString(" wbc = %1 rbc= %2 plt = %3").arg(statisticsWBC.at(i)).arg(statisticsRBC.at(i)).arg(statisticsPLT.at(i));
    }
      ui->widgetWBC->resetData(statisticsWBC);
      qApp->processEvents();
      ui->widgetRBC->resetData(statisticsRBC);
      qApp->processEvents();
      ui->widgetPLT->resetData(statisticsPLT);
      qApp->processEvents();
}

void MainWindow::findCrestList(int &iStart, QVector<int> &sourceList, QVector<int> &crestList, QVector<int> &vallyList)
{
    QVector<int> diff(sourceList.size()-1,0);
    //计算一阶差分
    for (QVector<int>::size_type i = 0; i !=diff.size(); i++)
    {
        if (sourceList[i + 1] - sourceList[i]>0)
            diff[i] = 1;
        else if (sourceList[i + 1] - sourceList[i] < 0)
            diff[i] = -1;
        else
            diff[i] = 0;
    }
    //遍历
    for (int i = diff.size()-1; i >= 0; i--)
    {
        if(diff[i] == 0 && i == diff.size()-1)
        {
            diff[i] = 1;
        }
        else if(diff[i] == 0)
        {
            if(diff[i+1] >= 0)
            {
                diff[i] =1;
            }
            else
            {
                diff[i] = -1;
            }
        }
    }
    //查找波峰
    for (QVector<int>::size_type i = 0;i!= diff.size()-1; i++)
    {
        //-2 波峰 2波谷
        if(diff[i+1] - diff[i] == -2)
        {
            //qDebug()<<QString("findCrestList %1").arg(i+1+iStart+1);
            crestList.push_back(i+1+iStart);
        }
        if(diff[i+1] - diff[i] == 2)
        {
            //qDebug()<<QString("findCrestList vally %1").arg(i+1+iStart+1);
            vallyList.push_back(i+1+iStart);
        }
    }

}


void MainWindow::on_btnLoadExcel_clicked()
{
    QString rootPath = "F:/testFile";

#ifdef Q_OS_ANDROID
    rootPath = "/storage/";
#endif
    FrmLoadFile::Instance()->setRootPath(rootPath,false,FrmLoadFile::Mode_Excel);
    FrmLoadFile::Instance()->showFrm();
}

void MainWindow::slotSelectedFile(QString file)
{
    this->excelPath = file;
    ui->txtExcelpath->setText(file);
    FrmLoadFile::Instance()->close();

    this->parseFile();
}
