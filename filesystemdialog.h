#ifndef FILESYSTEMDIALOG_H
#define FILESYSTEMDIALOG_H
#include "FileTreeView.h"
#include<QDialog>
#include<QString>
#include<QTreeView>
#include<QFileSystemModel>
#include<QMouseEvent>

//遍历系统文件目录对话框
class CFileSystemDialog : public QDialog
{
    Q_OBJECT
public:
    CFileSystemDialog();

private:
    QString path;
    CFileTreeView *treeView;
signals:
    void signalPathSelected(QString string);
};

#endif // FILESYSTEMDIALOG_H
