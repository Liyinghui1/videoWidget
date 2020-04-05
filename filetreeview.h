#ifndef FILETREEVIEW_H
#define FILETREEVIEW_H

#include<QWidget>
#include<QTreeView>
#include<QString>
#include<QMouseEvent>
#include<QFileSystemModel>

//获得系统文件目录
class CFileTreeView : public QTreeView
{
    Q_OBJECT
public:
    CFileTreeView();
    void mousePressEvent(QMouseEvent *event);
private:
    QFileSystemModel *model;
signals:
    void signalFilePathSelected(QString string);
};

#endif // FILETREEVIEW_H
