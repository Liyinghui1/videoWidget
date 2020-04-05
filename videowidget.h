#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "MySlider.h"
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QPoint>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QListWidget>
#include <QPushButton>
#include <QVideoWidget>
#include <QContextMenuEvent>
#include <QTimer>
#include <QTime>

#include <QMediaPlayer>

//用于显示视频的窗口类
class CVideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    explicit CVideoWidget(QWidget *parent = 0);

protected:
    bool    eventFilter(QObject* target, QEvent * event);  //事件过滤
    void    mouseMoveEvent(QMouseEvent *);
    void    mouseDoubleClickEvent(QMouseEvent *);
    void    mouseReleaseEvent(QMouseEvent *);
    void    mousePressEvent(QMouseEvent *);
    void    contextMenuEvent(QContextMenuEvent * event);
    void    dragEnterEvent(QDragEnterEvent* event);
    void    dropEvent(QDropEvent*event);
    void    keyPressEvent(QKeyEvent* event);

signals:
    void signalInBottomRegion();
    void signalOutBottomRegion();
    void signalInRightRegion();
    void signalOutRightRegion();
    void signalMovePoint(QPoint);
    void signalLeftPressDown(bool);   
    void signalDropedList(QList<QUrl> urls);
    void signalActionOpen();
    void signalActionInfo();
    void signalActionCapture();
    void signalMouseRelease();
    void signalDoubleClickedOrKeyEscape(bool bIsDoubleClicked);
    void signalKeySpace();

private:
    enum Direction { UP=0, DOWN=1, LEFT, RIGHT, LEFTTOP, LEFTBOTTOM, RIGHTBOTTOM, RIGHTTOP, NONE };
    const int PADDING ;
    const int buttonWidth ;
    const int buttonHeight ;
    const int windowIniWidth;
    const int windowIniHeight;

    QRect   normalRect;             //保存最大化以前的大小，因为最小化以前this->rect()已经变成了全屏时的rect()
    bool    m_bIsLeftPressDown;     //判断左键是否按下

    QPoint  m_dragPosition;           //窗口移动拖动时需要记住的点
    Direction dir;                  //窗口大小改变时，记录改变方向

    int fullScreenWidth;
    int fullScreenHeight;
    int clickCount;
    bool b_fulScreen;

public slots:

private:
   void     onMousePress();
   void     slotTimerTimeOut();
   void     slotSetCursorVisible();

public:
   void     setMediaPlayer(QMediaPlayer* player);
   bool     getBottomWidgetVisible();
   bool     getHideBottonVisible();

private:
   QMediaPlayer*    m_mediaPlayer;     //播放者
   QTimer           m_Timer;
   QTimer           m_setCursorVisible;
   QPoint           m_tmpPoint;
   bool             m_bIsBottomWidgetShowing;
   bool             m_bIsHideBottonShowing;

   bool             m_bIsDragVideoWidget;
   QPoint           m_LeftPressPoint;
   QTime            m_startTime;

};

#endif // MAINWIDGET_H
