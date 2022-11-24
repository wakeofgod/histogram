#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "frmloadfile.h"
#include "scatterchart3d.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void parseFile();
    void findCrestList(int &iStart, QVector<int> &sourceList,QVector<int> &crestList,QVector<int> &vallyList);

    QString  excelPath;

    QFile *file;

    QVector<int> zeroSourceList;
    QVector<int> tenSourceList;
    QVector<int> ninetySourceList;
    //波峰
    QVector<int>zeroCrestList;
    QVector<int>tenCrestList;
    QVector<int>ninetyCrestList;
    //波谷
    QVector<int> zeroVallyList;
    QVector<int> tenVallyList;
    QVector<int> ninetyVallyList;
    //波长
    QVector<int>zeroWaveList;
    QVector<int>tenWaveList;
    QVector<int>ninetyWaveList;

    //
    int zeroMax = 0,tenMax = 0,ninetyMax = 0;
    QVector<int> statisticsWBC;
    QVector<int> statisticsRBC;
    QVector<int> statisticsPLT;


private slots:
    void on_btnLoadExcel_clicked();

    void slotSelectedFile(QString file);

private:
    Ui::MainWindow *ui;
    ScatterChart3D *scatter;
};
#endif // MAINWINDOW_H
