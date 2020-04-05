#ifndef WIDGET_H
#define WIDGET_H

#include "VideoWidget.h"
#include "MySlider.h"
#include "BottomWidget.h"
#include "ListWidget.h"
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>
#include <QGroupBox>
#include <QString>
#include <QMap>
#include <QPushButton>
#include <QModelIndex>
#include<QMediaPlayer>
#include <QVideoWidget>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QEvent>
#include <QTimer>
#include <QObject>
#include <QMouseEvent>
#include <QMenu>
#include <QVector>
#include <QPair>
#include <QMultiMap>
#include <QListWidget>
#include <QAction>
#include <QSqlTableModel>
#include <QMediaPlaylist>
#include "topWidget.h"
#include <QTime>
#include "SplitLine.h"
#include "MyToolTip.h"
#include "rightWidget.h"

namespace Ui {
class Widget;
}

//界面的主窗口
class CWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CWidget(QWidget *parent = 0);
    ~CWidget();

protected:
    void    mouseDoubleClickEvent(QMouseEvent *);
    void    mouseMoveEvent(QMouseEvent *);
    void    mouseReleaseEvent(QMouseEvent *);
    void    mousePressEvent(QMouseEvent *);
    void    leaveEvent(QEvent *);
    void    enterEvent(QEvent *);
    void    keyPressEvent(QKeyEvent *event);
    void    paintEvent(QPaintEvent *event);
    void    resizeEvent(QResizeEvent *event);
    bool    eventFilter(QObject* target, QEvent * event);  //事件过滤
    void    showEvent(QShowEvent *event);

public:
    int     getWindowIniWidth() const {return m_windowIniWidth;}
    int     getWindowIniHeight()const {return m_windowIniHeight; }
    void    region(const QPoint &cursorGlobalPoint);       //按照九宫格的思想判断鼠标现在的位置
    QString getNameFromPath(const QString &fileName);      //从路径中获取文件名
    QString digitalToTime(const int& number);              //把时间转换为标准格式
    QString removeIndexFromAppend( QString name);          //移除append序号
    void    getDataFromDatabase();                                 //从数据库读数据到窗体
    void    updateToDataBase();                            //根据vector更新数据库
    void    addFile(const QString filePath);               //根据路径名添加文件
private:
    Ui::Widget *ui;

    enum Direction
    {
        UP=0,
        DOWN=1,
        LEFT,
        RIGHT,
        LEFTTOP,
        LEFTBOTTOM,
        RIGHTBOTTOM,
        RIGHTTOP,
        NONE
    };  //九宫格方向

    const   int PADDING;                        //用于边缘判断
    const   int m_windowIniWidth;               //窗口初始化的宽度
    const   int m_windowIniHeight;              //窗口初始化的高度
    const   int m_screenWidth;                  //屏幕的宽度
    const   int m_screenHeight;                 //屏幕的高度
    static  QPoint  Position;                   //通过bottomWidget或者videoWidget移动窗体时，记录窗体移动前的位置

    //窗体
    QRect           normalRect;                 //保存最大化以前的大小，因为最小化以前this->rect()已经变成了全屏时的rect()
    bool            m_bIsLeftPressDown;         //判断左键是否按下
    QPoint          dragPosition;               //窗口移动拖动时需要记住的点
    Direction       dir;                        //窗口大小改变时，记录改变方向
    QPushButton*    m_closeButton;              //关闭按钮
    QPushButton*    m_minimumButton;            //最小化按钮
    QPushButton*    m_maximumButton;            //最大化按钮
    QWidget*        m_videoWidgetBase;          //将videoWidget放在此上面
    CBottomWidget*  m_bottomWidget;             //底部控件，用于放下一曲上一曲等
    CListWidget*    m_listWidget;               //显示列表控件
    CTopWidget*     m_topWidget;                //顶部控件，用于放标题等控件
    QLabel*         m_titleLabel;               //标题栏

    QPoint          m_mouseGlobalPos;           //鼠标全局坐标
    QWidget*        m_hideButtonBase;           //播放列表的隐藏按钮
    QPushButton*    m_hideButton;
    CRightWidget*   m_rightWidget;
    QLabel*         m_listLabel;

    QStringList     m_playingModeList;
    QString         m_playingMode;

    //菜单
    QPushButton*    m_menuButton;
    QPushButton*    m_menuButton2;
    QMenu*          m_menu;
    QAction*        m_openAction;

    //播放器控件
    QMediaPlayer*       m_mediaPlayer;              //播放者
    QMediaPlaylist*     m_mediaPlaylist;            //播放列表
    CVideoWidget*       m_videoWidget;              //显示视频
    QLabel*             m_mainLabel;                //用于视频暂停时显示画面，从而 避免移动时黑屏

    QPushButton*    m_stopButton;
    QPushButton*    m_previousVideoButton;
    QPushButton*    m_playButton;
    QPushButton*    m_nextVideoButton;
    QPushButton*    m_playModeButton;
    QPushButton*    m_volumeButton;
    QPushButton*    m_fullScreenButton;
    QPushButton*    m_addButton;

    CMySlider*      m_volumeSlider;
    CMySlider*      m_timeSlider;
    QLabel*         m_timeLabel;


signals:
    void    signalInRegion();
    void    signalOutRegion();

public slots:
    void    slotSetMaxButton();
    void    slotOpenFile();
    void    slotStateChanged(QMediaPlayer::State);
    void    slotDurationChanged(qint64);
    void    slotPositionChanged(qint64);
    void    slotPlayOrPause();
    void    slotNextVideo();
    void    slotPreviousVideo();
    void    slotStopVideo();

    void    slotInBottomRegion();
    void    slotOutBottomRegion();
    void    slotOutBottomRegionTimer();
    void    slotSignalPressPosition(qint64 position);
    void    slotMouseMovePosition(qint64 position);
    void    slotPressVolumePosition(qint64 position);
    void    slotListWidgetItemDoubleClick(int currentIndex);
    void    slotListWidgetItemDeleted(int n);
    void    slotSignalMovePoint(QPoint );
    void    slotVideoWidgetMouseLeftDown();
    void    slotHandleError();

    void    slotListWidgetClearAll();
    void    slotHideButton();
    void    slotInRightRegion();
    void    slotOutRightRegion();
    void    slotOutRightRegionTimer();
    void    slotCloseWidget();
    void    slotVideoWidgetDropedList(QList<QUrl>urls);
    void    slotListWidgetClearValidItem();
    void    slotListWidgetOpenDirectory(int n);
    void    slotVideoWidgetActionInfo();
    void    slotVideoWidgetActionCapture();
    void    slotBottomWidgetDoubleClicked();

    void    slotVideoWidgetDoubleClickedOrKeyEscape(bool bIsDoubleClicked);
    void    slotVideoWidgetKeySpace();
    void    slotPlayMode();
    void    setPlayModel();
    void    slotFullScreenButtonClicked();

    void    slotPlaylistChanged(int position);
    void    slotListWidgetAddMediaFile(QStringList strList);
    void    slotVolumeButtonCliced();

    void    slotCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void    slotCureentItemDoubleClick(QListWidgetItem *current);
    void    slotVolumeToolTipTimer();

private:
    void    initTopWidget();
    void    initBottomWidget();
    void    initVideoWidget();
    void    initRightWidget();
    void    initMediaPlayer();

    void    refreshLayout(QRect rectClient);
    void    bottomWidgetLayout();
    void    topWidgetLayout();
    void    videoWidgetLayout();
    void    rightWidgetLayout();

    void    setFullScreen(bool bIsFullScreen);
    void    setState(bool bIsFullScreen);
    QString    timeFormat(qint64 secs);
    void    isMuteVisible(bool bIsMute);

    void    setBottomWidgetFlags(bool bIsTool);
    void    setRightWidgetFlags(bool bIsTool);
    bool    isFileExist(QString fullFileName);
    void    openTable();   
    void    showVolumeToolTip(int value);


private:
    QRect       m_rectTopWidget;
    QRect       m_rectVideoWidget;
    QRect       m_rectBottomWidget;
    QRect       m_rectRightWidget;
    QRect       m_rectListWidget;
    QRect       m_rectHideButton;   //隐藏按钮的rect
    QRect       m_rectListLabel;
    QRect       m_rectSplitLineTop;
    QRect       m_rectSplitLineBottom;

    QSize       m_sizeHideButton;   //隐藏按钮的宽和高
    QSize       m_sizeTopWidget;
    QSize       m_sizeListWidget;
    QSize       m_sizeBottomWidget;
    QSize       m_sizeVideoWidget;
    QSize       m_sizeRightWidget;
    QSize       m_sizeListLabel;
    QSize       m_sizeSplitLineTop;
    QSize       m_sizeSplitLineBottom;

    QMenu*      m_menuPlayMode;
    QAction*    m_actSinglePlay;
    QAction*    m_actSingleCycle;
    QAction*    m_actOrderPlay;
    QAction*    m_actLoopPlay;
    QAction*    m_actShufflePlay;

    QString     m_durationTime;     //总长度
    QString     m_positionTime;     //当前播放列表的位置

    QTimer*     m_outRightRegionTimer;
    QTimer*     m_outRegionTimer;
    QTime       m_bottomWidgetStartTime;
    QTime       m_hideBottonStartTime;

    CSplitLine*  m_splitLineTop;
    CSplitLine*  m_splitLineBottom;
    QMenu*      m_menuAdd;
    QAction*    m_actionAddFile;
    QAction*    m_actionAddFolder;

    QVector<qint64>   m_vectorProgress;        //保存媒体进度
    int         m_volumePos;
    CMyToolTip*  m_volumeToolTip;
    QPoint      m_volumeToolTipPos;
    QTimer*     m_volumeToolTipTimer;
    QTime       m_startVolumeToolTipTime;

    QWidget*    m_bottomWidgetBase;
    QWidget*    m_rightWidgetBase;
    //QWidget*    m_splitLineTopBase;
    //QWidget*    m_splitLineBottomBase;


};



#endif // WIDGET_H
