#ifndef FRMLOADFILE_H
#define FRMLOADFILE_H

#include <QWidget>
#include <QtCore>
#include <QStandardItemModel>

namespace Ui {
class FrmLoadFile;
}

class FrmLoadFile : public QWidget
{
    Q_OBJECT

public:
    explicit FrmLoadFile(QWidget *parent = nullptr);
    ~FrmLoadFile();

    enum LoadFile_Mode
    {
        Mode_DBScript = 0,
        Mode_DbFile = 1,
        Mode_KeyFile = 2,
        Mode_LogFile = 3,
        Mode_PrintTempt = 4,
        Model_Hex = 5,
        Model_ImportDir = 6,
        Model_Import_Par_Dir = 7,
        Mode_APkFile = 8,
        Mode_Excel = 9
    };

    static FrmLoadFile* Instance();

    void showFrm();

    void hideFrm();

    void initFrm();

    void setRootPath(const QString& filePath,bool isLock=true,LoadFile_Mode mode=FrmLoadFile::Mode_Excel);

    void loadTV();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void signalSelectedFile(QString file);

private slots:
    void on_btnPrevious_clicked();

    void on_btnNext_clicked();

    void slotAnimationFinish();

    void slotFileTvDoubleClicked(const QModelIndex& index);

private:
    Ui::FrmLoadFile *ui;
    static FrmLoadFile* self;
    QPropertyAnimation* animation;

    QStandardItemModel* itemModel;
    QString mPath;
    QString mPathPrevious;
    bool isLockRootPath;
    LoadFile_Mode mMode;
    bool isHide;
};

#endif // FRMLOADFILE_H
