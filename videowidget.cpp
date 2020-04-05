#include "VideoWidget.h"

#include<QWidget>
#include<QDebug>
#include<QColor>
#include<QVideoWidget>
#include<QPoint>
#include<QList>
#include<QUrl>
#include<QMenu>
#include<QAction>
#include<QString>
#include<QTimer>
#include<QMimeData>
#include<QCursor>
#include<QVBoxLayout>
#include<QContextMenuEvent>
#include<QDragEnterEvent>
#include<QDropEvent>
#include <QTime>

CVideoWidget::CVideoWidget(QWidget *parent) : QVideoWidget(parent), PADDING(2), buttonHeight(20),buttonWidth(20),  windowIniHeight(600), windowIniWidth(800),clickCount(1),m_bIsLeftPressDown(false),
    dir(NONE),b_fulScreen(false)
{
    this->setMouseTracking(true);
    this->setCursor(QCursor(Qt::ArrowCursor));
    this->setAutoFillBackground(true);
    this->setStyleSheet("QVideoWidget{background-color:rgb(0,0,0);}");
    setWindowOpacity(1);//设置透明度
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint); // 设置成无边框对话框
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setAcceptDrops(true);    //设置可以接受拖拽

    this->installEventFilter(this);

    connect(&m_Timer, &QTimer::timeout, this, &CVideoWidget::slotTimerTimeOut);
    connect(&m_setCursorVisible, &QTimer::timeout, this, &CVideoWidget::slotSetCursorVisible);

    //setAttribute(Qt::WA_TranslucentBackground, true);
    //setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    m_bIsBottomWidgetShowing = false;
    m_bIsHideBottonShowing = false;

    m_bIsDragVideoWidget = false;
    m_LeftPressPoint.setX(0);
    m_LeftPressPoint.setY(0);

}

//事件过滤器
bool CVideoWidget::eventFilter(QObject* target, QEvent *event)
{
    return QVideoWidget::eventFilter(target, event);
}

void CVideoWidget::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu *menu = new QMenu(this);
    menu->setStyleSheet(" QMenu{border:none ;} "
                                          "QMenu::item{border:none; color:rgb(30,30,30); background-color:rgb(160,160,160); font-size:13px;  padding-left:20px; "
                                                                   "background-color:white; height:28px; width:120px; }"
                                          "QMenu::item:selected{color:rgb(255,255,255); background-color:rgb(140,140,240)}; "
                        );
    QAction *actionOpen =  menu->addAction("打开");
    QAction *actionCapture = menu->addAction("截图");
   // QAction *actionInfo    =  menu->addAction("媒体信息");
    //QAction *action

    connect(actionOpen, &QAction::triggered, [this](){emit signalActionOpen(); });
    connect(actionCapture, &QAction::triggered, [this](){emit signalActionCapture(); });
   // connect(actionInfo, &QAction::triggered, [this](){emit signalActionInfo(); });

    menu->exec(QCursor::pos());
}

void CVideoWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint topLeftPoint(0, this->height()/8*7);
    QPoint rightBottomPoint(this->width(), this->height());
    topLeftPoint  = this->mapToGlobal(topLeftPoint);
    rightBottomPoint = this->mapToGlobal(rightBottomPoint);
    QPoint mouseGlobalPoint = this->mapToGlobal(event->pos());

    QPoint tmpPoint (this->width(), this->height()/2);
    tmpPoint = this->mapToGlobal(tmpPoint);

    if(!m_bIsLeftPressDown)  //如果左键没有按下，则更新鼠标相对于Widget的位置信息
    {
        //是否到达设定的区域使bottomWidget隐藏
        if(this->isFullScreen())
        {
            if(topLeftPoint.y() < mouseGlobalPoint.y())   //显示bottomWidget
            {
                m_bIsBottomWidgetShowing = true;
                emit signalInBottomRegion();
            }
            else           //隐藏bottomWidget
            {
                m_bIsBottomWidgetShowing = false;
                emit signalOutBottomRegion();
            }
        }

        //是否到达设定的区域使hideButton隐藏
        if(tmpPoint.x() - mouseGlobalPoint.x() < 400)  //显示hideButton
        {
            m_bIsHideBottonShowing = true;
            emit signalInRightRegion();
        }
        else  //隐藏hideButton
        {
            m_bIsHideBottonShowing = false;
            emit signalOutRightRegion();
        }
    }
    else
    {
        emit signalMovePoint(event->globalPos() - m_dragPosition);
    }

    //隐藏鼠标指针
    if(!m_setCursorVisible.isActive())
    {
        m_setCursorVisible.start(50);
        this->setCursor(Qt::ArrowCursor); //显示正常鼠标
    }
    m_startTime = QTime::currentTime();
}

void CVideoWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bIsLeftPressDown = false;
        if(m_LeftPressPoint.x() == event->globalX() && m_LeftPressPoint.y() == event->globalY())
        {
            m_Timer.start(200);
        }

        if(dir != NONE)
        {
            this->releaseMouse();
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void CVideoWidget::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::LeftButton:
        m_bIsLeftPressDown = true;
        emit signalLeftPressDown(true);
        m_LeftPressPoint = event->globalPos();

        if(dir != NONE)
        {
            this->mouseGrabber();
        }
        else
        {
            m_dragPosition = event->globalPos();
        }

        break;

    case Qt::RightButton:
        break;

    default:
        break;
    }
}

void CVideoWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_Timer.stop();
        emit signalDoubleClickedOrKeyEscape(true);
    }
}

void CVideoWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void CVideoWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        return;
    }
    emit signalDropedList(urls);
}

void CVideoWidget::onMousePress()
{
    emit signalMouseRelease();
}

void CVideoWidget::setMediaPlayer(QMediaPlayer* player)
{
    m_mediaPlayer = player;
}

bool CVideoWidget::getBottomWidgetVisible()
{
    return m_bIsBottomWidgetShowing;
}

bool CVideoWidget::getHideBottonVisible()
{
    return m_bIsHideBottonShowing;
}

void CVideoWidget::slotTimerTimeOut()
{
    m_Timer.stop();
    onMousePress();
}

void CVideoWidget::slotSetCursorVisible()
{
    int msecsCount = m_startTime.msecsTo(QTime::currentTime());
    if(msecsCount > 3000)
    {
        this->setCursor(Qt::BlankCursor);//隐藏鼠标
        m_setCursorVisible.stop();
    }
}

void CVideoWidget::keyPressEvent(QKeyEvent* event)
{//按键事件，ESC退出全屏状态
    int keyMsg = event->key();

    if((keyMsg == Qt::Key_Escape) && isFullScreen())
    {
        emit signalDoubleClickedOrKeyEscape(false);
        return;
    }
    QVideoWidget::keyPressEvent(event);
}
