#include "FileTreeView.h"
#include<QPushButton>
#include<QMouseEvent>
#include<QDebug>
#include<QFileInfo>
#include<QFileSystemModel>

CFileTreeView::CFileTreeView()
{
    model= new QFileSystemModel;
    model->setRootPath("C:\\");
    this->setModel(model);
}

void CFileTreeView::mousePressEvent(QMouseEvent *event)
{
    QPoint point  = event->pos();
    QModelIndex modelIndex = this->indexAt(point);
    if(event->button() == Qt::LeftButton)
    {
        if(modelIndex.isValid())
        {
            QString filePath = model->filePath(modelIndex);
            emit signalFilePathSelected(filePath);
        }
    }
    QTreeView::mousePressEvent(event);
}
