#include "ListWidget.h"
#include<QScrollBar>
#include<QPalette>
#include<QColor>
#include<QDebug>
#include<QMouseEvent>
#include<QString>
#include<QUrl>
#include<QListWidget>
#include<QListWidgetItem>
#include<QMenu>
#include<QFileDialog>
#include<QAction>
#include<QCursor>
#include<QDesktopServices>
#include <QStyleOption>

CListWidget::CListWidget(QWidget *parent):QListWidget(parent), m_currentIndex(0)
{
    //播放列表水平滚动条
    QScrollBar* horizontalScroll = new QScrollBar(Qt::Horizontal);
    QPalette palette(QColor(70,70,70));
    horizontalScroll->setStyleSheet("QScrollBar{background-color: rgb(60,65,68);"
                                                "width: 5px;}"
                                    "QScrollBar::handle{background-color:rgb(110,110,110);}"
                                    "QScrollBar::handle:hover{background-color:rgb(160,160,160);}"
                                    );
    horizontalScroll->setPalette(palette);

    //播放列垂直滚动条
    QScrollBar* verticalScroll = new QScrollBar(Qt::Vertical);
    QPalette palette2(QColor(70,70,70));
    //scroll->setAutoFillBackground(true);
    verticalScroll->setStyleSheet("QScrollBar{background-color: rgba(26, 26, 26, 0);"
                                              "width: 10px;}"
                                  "QScrollBar::handle{border-right:5px; "
                                                      "background-color:rgba(60, 60, 60, 255);}"
                                  "QScrollBar::handle:hover{background-color:rgb(160, 160, 160);"

                                  "QScrollBar::add-page{background-color:rgba(26, 26, 26, 255);"
                                                        "border-radius:4px;}"
                                  "QScrollBar::sub-page{background-color:rgba(26, 26, 26, 255);"
                                                        "border-radius:4px;}"
                                  );


    verticalScroll->setPalette(palette2);

    //color:rgb(180,180,255);
    this->setStyleSheet("QListWidget{border:0px;"
                                     "background-color:rgba(26,26,26,0)}"
                        "QListWidget::item:hover{background-color:rgb(47,47,47); "
                                                 "outline:0px;}"
                        "QListWidget::item{padding-left:25px;"
                                           "padding-top:0px;"
                                           "padding-bottom:0px;"
                                           "padding-right:20px;"
                                           "background-color:rgba(26, 26, 26, 0);"
                                           "height:40px;"
                                           "width:108px;}"
                        "QListWidget::item:selected{background-color:rgb(62,62,62);"
                                                    "color:rgba(140, 140, 140, 255);"
                                                    "outline:0px;}"
                        );
    //
    this->setFocusPolicy(Qt::NoFocus);
    this->setHorizontalScrollBar(horizontalScroll);
    this->setVerticalScrollBar(verticalScroll);
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  //隐藏滚动条
    this->setMouseTracking(true);
    this->setCursor(QCursor(Qt::ArrowCursor));

//    connect(this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
//            this, SLOT(slotCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    connect(this, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(slotItemPressed(QListWidgetItem*)));

}

void CListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {    
        int value = this->verticalScrollBar()->value();
        int currentIndex = event->pos().y()/40 + value; //40为item区域的高度

        emit ItemDoubleClick(currentIndex);
    }
    QListWidget::mouseDoubleClickEvent(event);
}


void CListWidget::mousePressEvent(QMouseEvent *event)
{
    //int currentIndex = event->pos().y()/40; //40为item区域的高度
    //QListWidgetItem* item = this->itemAt(event->pos());
    //item->setBackground(Qt::white);

    //QListWidget::mousePressEvent(event);

    //QListWidgetItem *item = itemAt(event->pos());
    //m_curTextColor = item->textColor();
    QListWidget::mousePressEvent(event);
}

void CListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidgetItem *item = itemAt(event->pos());
    //item->setTextColor(m_curTextColor);
    setCurrentItem(item);
    QListWidget::mouseReleaseEvent(event);
}

void CListWidget::contextMenuEvent(QContextMenuEvent *menuEvent)
{    
    QListWidgetItem* item = this->itemAt(menuEvent->pos());

    QMenu * menu = new QMenu(this);
    menu->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    menu->setAttribute(Qt::WA_TranslucentBackground);

    menu->setStyleSheet("QMenu{border:none;"
                               "border-radius: 5px;"
                               "background-color:white;}"
                        "QMenu::item{border:none;"
                                    "border-radius: 10px;"
                                    "font-size:16px;"
                                    "padding-left:30px; "
                                    "background-color:white; "
                                    "height:40px;"
                                    "width:150px;}"
                        "QMenu::item:selected{background-color:rgb(140,140,240)};"
                        );

    QAction* actionAddFile          = menu->addAction("添加文件");
    QAction* actionAddFolder        = menu->addAction("添加文件夹");

    if(NULL != item)
    {
        int value = this->verticalScrollBar()->value();
        m_currentIndex = menuEvent->pos().y()/40 + value;//40为item区域的高度

        QAction* actionPlay             = menu->addAction("播放");
        QAction* actionOpenDirectory    = menu->addAction("打开文件所在目录");
        QAction* actionDelete           = menu->addAction("删除");

        connect(actionPlay, &QAction::triggered, this, &CListWidget::slotActionPlay);
        connect(actionOpenDirectory, &QAction::triggered, this, &CListWidget::slotActionOpenDirectory);
        connect(actionDelete, SIGNAL(triggered()), this, SLOT(slotActionDeleteItem()));
    }

    QAction* actionClearValidItem   = menu->addAction("删除失效记录");
    QAction* actionClearAll         = menu->addAction("清空列表");

    connect(actionClearAll, SIGNAL(triggered(bool)), this, SLOT(slotActionClearAll()));
    connect(actionClearValidItem,&QAction::triggered, this, &CListWidget::slotActionClearValidItem);
    connect(actionAddFile, &QAction::triggered, this, &CListWidget::slotActionAddFile);
    connect(actionAddFolder, &QAction::triggered, this, &CListWidget::slotActionAddFolder);

    menu->exec(mapToGlobal(menuEvent->pos()));  //或者menu->exec(QCursor::pos());
    QListWidget::contextMenuEvent(menuEvent);
}

void CListWidget::paintEvent(QPaintEvent *event)
{
    QListWidget::paintEvent(event);
}

void CListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent(event);
}

void CListWidget::slotActionDeleteItem()
{
    emit ItemDeleted(m_currentIndex);
}

void CListWidget::slotActionClearValidItem()
{
    emit signalClearValidItem();
}

void CListWidget::slotActionClearAll()
{
    emit signalClearAll();
}

void CListWidget::slotActionPlay()
{
    emit ItemDoubleClick(m_currentIndex);
}

void CListWidget::slotActionAddFile()
{//添加文件
    QString curPath = QDir::homePath();//获取用户目录
    QString dlgTitle = "选择音频文件";
    QString filter = "所有文件(*.*);;音频文件(*.mp3 *.wav *.wma);;mp3文件(*.mp3);;wav文件(*.wav);;"
                     "wma文件(*.wma);;wkv文件(*.mkv)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, dlgTitle, curPath, filter);
    int strCount = fileList.count();
    if(strCount < 1)
    {
        return;
    }

    emit signalAddMedia(fileList);
}

void CListWidget::slotActionAddFolder()
{

}

void CListWidget::slotCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    qDebug()<<"CListWidget::slotCurrentItemChanged";
}

void CListWidget::slotItemPressed(QListWidgetItem *current)
{
    QColor color = current->textColor();
}

void CListWidget::slotActionOpenDirectory()
{
    emit signalOpenDirectory(m_currentIndex);
}
