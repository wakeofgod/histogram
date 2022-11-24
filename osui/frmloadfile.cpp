#include "frmloadfile.h"
#include "ui_frmloadfile.h"
#include <QScreen>

FrmLoadFile* FrmLoadFile::self = nullptr;
FrmLoadFile * FrmLoadFile::Instance()
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new FrmLoadFile;
        }
    }

    return self;
}

FrmLoadFile::FrmLoadFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmLoadFile)
{
    ui->setupUi(this);
    this->animation = new QPropertyAnimation(this,"pos");
    this->animation->setDuration(500);
    this->itemModel = new QStandardItemModel(this);

    initFrm();
    connect(this->animation,SIGNAL(finished()),this,SLOT(slotAnimationFinish()));
    connect(ui->tvFile,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotFileTvDoubleClicked(QModelIndex)));
}

FrmLoadFile::~FrmLoadFile()
{
    delete ui;
}


void FrmLoadFile::initFrm()
{
    ui->tvFile->setModel(this->itemModel);
    //初始化参数表格
    //自动铺满
    ui->tvFile->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);
    //ui->tvFile->horizontalHeader()->setStretchLastSection(true);
    //选中一整行
    ui->tvFile->setSelectionBehavior(QAbstractItemView::SelectRows);
    //只能选一行
    ui->tvFile->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvFile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //滚动条 被全局样式影响
    ui->tvFile->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tvFile->setAlternatingRowColors(true);
    ui->tvFile->setShowGrid(false);
    ui->tvFile->verticalHeader()->hide();
    //ui->tvFile->horizontalHeader()->hide();
    ui->tvFile->verticalHeader()->setDefaultSectionSize(34);
    ui->tvFile->verticalHeader()->setMinimumSectionSize(34);
    //标题背景色
    ui->tvFile->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(0,248,248)}");

    QStringList header;
    header<<tr("文件名称")<<tr("创建时间");
    this->itemModel->setHorizontalHeaderLabels(header);

    ui->btnPrevious->setToolTip(tr("上级目录"));
    ui->btnNext->setToolTip(tr("返回"));
}

bool FrmLoadFile::eventFilter(QObject *obj, QEvent *event)
{
    return QWidget::eventFilter(obj, event);
}

void FrmLoadFile::showFrm()
{
    this->show();
    this->loadTV();
    QScreen *screen = QGuiApplication::primaryScreen ();
    QRect screenRect =  screen->availableVirtualGeometry();
    this->animation->setStartValue(QPoint(screenRect.width()/2-this->width()/2,0));
    this->animation->setEndValue(QPoint(screenRect.width()/2-this->width()/2,screenRect.height()/2-this->height()/2));

    this->animation->start();
    this->isHide = false;
}

void FrmLoadFile::hideFrm()
{

}

void FrmLoadFile::setRootPath(const QString &filePath, bool isLock, LoadFile_Mode mode)
{
    this->mPath = filePath;
    this->mPathPrevious = filePath;
    this->isLockRootPath = isLock;
    this->mMode = mode;
    ui->txtFilePath->setText(this->mPath);
}

void FrmLoadFile::loadTV()
{
    this->itemModel->removeRows(0,this->itemModel->rowCount());
    QString filePath = ui->txtFilePath->text();
    if(filePath.isEmpty() || filePath.isNull())
    {
        return;
    }
    QDir dir;
    dir.setPath(filePath);
    dir.setFilter(QDir::AllEntries);
    QStringList names = dir.entryList();

    for(QString str: names)
    {
        if(str=="." || str=="..")
        {
            continue;
        }
        if(str == "self" || str == "emulated")
        {
            continue;
        }
        int count = this->itemModel->rowCount();
        this->itemModel->insertRow(count);

        if(filePath[filePath.length()-1]=='/')
        {
            filePath = filePath.left(filePath.length()-1);
        }
        QString tfile = QString("%1/%2").arg(filePath).arg(str);
        QFileInfo fileInfo(tfile);
        this->itemModel->setItem(count,0,new QStandardItem(str));
        this->itemModel->setItem(count,1,new QStandardItem(fileInfo.created().toString("yyyy-MM-dd hh:mm:ss")));
    }
}

void FrmLoadFile::on_btnPrevious_clicked()
{
    QString filePath = this->mPath;
    if(filePath.isEmpty() || filePath.isNull())
    {
        return;
    }
    if(filePath[filePath.length()-1]=='/')
    {
        filePath = filePath.left(filePath.length()-1);
    }
    //备份前一次路径
    this->mPathPrevious = filePath+'/';
    //去除最后一个文件夹
    filePath = filePath.left(filePath.lastIndexOf('/'));
    filePath = filePath+"/";

    //qDebug()<<QString("FrmLoadFile::on_btnPrevious_clicked %1  %2").arg(filePath).arg(this->mPath);
    QFileInfo fileInfo(filePath);
    if(fileInfo.isDir())
    {
        this->mPath = filePath;
        ui->txtFilePath->setText(filePath);
        this->loadTV();
    }
}

void FrmLoadFile::on_btnNext_clicked()
{
    this->mPath = this->mPathPrevious;
    this->ui->txtFilePath->setText(this->mPathPrevious);
    this->loadTV();
}

void FrmLoadFile::slotAnimationFinish()
{
    if(this->isHide)
    {
        this->hide();
    }
}

void FrmLoadFile::slotFileTvDoubleClicked(const QModelIndex &index)
{
    QString file = this->itemModel->data(itemModel->index(index.row(),0)).toString();
    QString filePath = ui->txtFilePath->text();
    if(filePath[filePath.length()-1]=='/')
    {
        filePath = filePath.left(filePath.length()-1);
    }

    filePath = QString("%1/%2").arg(filePath).arg(file);
    QFileInfo fi(filePath);
    if(fi.isFile())
    {
        emit this->signalSelectedFile(QString("%1/%2").arg(this->mPath).arg(file));
    }

    if(fi.isDir())
    {
        qDebug()<<"load dir......";
        this->ui->txtFilePath->setText(filePath);
        this->mPath = filePath;
        this->loadTV();
    }
}
