#include "mainWidget.h"
#include "ui_Widget.h"
#include "BottomWidget.h"
#include <CaptureDialog.h>
#include <VideoWidget.h>
#include <Windows.h>
#include <ShellAPI.h>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QPair>
#include <QPalette>
#include <QVector>
#include <QPushButton>
#include <QMultimedia>
#include <QSize>
#include <QGroupBox>
#include <QMediaPlayer>
#include <QModelIndex>
#include <QVideoWidget>
#include <QLabel>
#include <QScrollBar>
#include <QSizePolicy>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QListWidget>
#include <QCursor>
#include <QFileDialog>
#include <QMap>
#include <QGridLayout>
#include <QTimer>
#include <iterator>
#include <QMultiMap>
#include <QLabel>
#include <QComboBox>
#include <QDir>
#include <QUrl>
#include <QRect>
#include <QStyle>
#include <QProcess>
#include <QTextCodec>
#include <QMessageBox>
#include <QMediaContent>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVideoEncoderSettings>
#include <QMediaRecorder>
#include <QMediaResource>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QApplication>
#include <QDesktopWidget>
#include <QTime>
#include <QFileInfo>
#include <QDesktopServices>
#include <QScreen>
#include <QList>
#include <QFontMetrics>
#include "MyToolTip.h"
#include <QModelIndex>
#include <QFileInfo>

QPoint CWidget::Position(QPoint(0,0));

CWidget::CWidget(QWidget *parent) : QWidget(parent), PADDING(2), m_bIsLeftPressDown(false), dir(NONE), ui(new Ui::Widget),
                 m_windowIniWidth((QApplication::desktop()->availableGeometry().width())*29/48),
                 m_windowIniHeight((QApplication::desktop()->availableGeometry().height())*20/27),
                 m_screenWidth(QApplication::desktop()->availableGeometry().width()),
                 m_screenHeight(QApplication::desktop()->availableGeometry().height())
{
    ui->setupUi(this);

    this->setWindowFlags(/*Qt::Window | */Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                         Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint); // 设置成无边框对话框

    this->setAutoFillBackground(true);
    this->setStyleSheet("QWidget{background-color:rgba(0,0,0,255);}");
    //setWindowOpacity(1);//设置透明度
    this->setMinimumHeight(m_windowIniHeight);
    this->setMinimumWidth(m_windowIniWidth);

    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->installEventFilter(this);

    m_listWidget = NULL;
    m_videoWidget = NULL;
    m_rightWidgetBase = NULL;
    m_rightWidget = NULL;

    normalRect.setRect(0, 0, m_windowIniWidth, m_windowIniHeight);

    m_sizeTopWidget.setHeight(60);
    m_sizeRightWidget.setWidth(300);
    m_sizeListWidget.setWidth(m_sizeRightWidget.width());
    m_sizeBottomWidget.setHeight(105);
    m_sizeHideButton.setWidth(35);
    m_sizeHideButton.setHeight(120);
    m_sizeListLabel.setHeight(50);
    m_sizeSplitLineTop.setHeight(2);
    m_sizeSplitLineBottom.setHeight(2);

    //初始化界面布局
    refreshLayout(normalRect);

    //初始化TopWidget
    initTopWidget();

    //初始化ListWidget
    initRightWidget();

    //初始化bottomWidget
    initBottomWidget();

    //初始化VideoWidget
    initVideoWidget();

    //初始化播放者
    initMediaPlayer();

    m_durationTime = "00:00";
    m_positionTime = "00:00";

    m_outRightRegionTimer = new QTimer;
    m_outRegionTimer = new QTimer;
    connect(m_outRightRegionTimer, SIGNAL(timeout()), this, SLOT(slotOutRightRegionTimer()));
    connect(m_outRegionTimer, SIGNAL(timeout()), this, SLOT(slotOutBottomRegionTimer()));
    //m_outRightRegionTimer->setSingleShot(true);
    //m_outRegionTimer->setSingleShot(true);

    m_playingModeList<<"CurrentItemOnce"<<"CurrentItemInLoop"<<"Sequential"<<"Loop"<<"Random";
    m_playingMode = m_playingModeList.at(0);

    m_vectorProgress.clear();
    getDataFromDatabase();

    m_volumeToolTip = NULL;
    m_volumeToolTipTimer = new QTimer;
    connect(m_volumeToolTipTimer, &QTimer::timeout, this, &CWidget::slotVolumeToolTipTimer);
    m_volumeToolTipPos.setX(35);
    m_volumeToolTipPos.setY(35);
}

CWidget::~CWidget()
{
    updateToDataBase();
    delete ui;
}

void CWidget::slotBottomWidgetDoubleClicked()
{
    qDebug()<<"slotBottomWidgetDoubleClicked";
    slotSetMaxButton();
}

void CWidget::addFile(const QString filePath)
{
    if(!filePath.isEmpty())
    {
        QString fileName = getNameFromPath(filePath);   //从路径中获取文件名字
        int mediaCount = m_mediaPlaylist->mediaCount();
        bool bIsFileExist = false;
        for(int i = 0; i < mediaCount; i++)
        {
            if(m_mediaPlaylist->media(i).canonicalUrl().fileName() == fileName)   //该文件已经存在
            {
                bIsFileExist = true;
                break;
            }
        }

        if(!bIsFileExist)
        {
            m_mediaPlaylist ->addMedia(QUrl::fromLocalFile(filePath));
            m_vectorProgress.append(0);
            QListWidgetItem* item = new QListWidgetItem(fileName);
            item->setTextColor(QColor(255, 255, 255));
            item->setToolTip(fileName);
            m_listWidget->addItem(item);
        }
    }
}

//数字显示转换为时间显示
QString CWidget::digitalToTime(const int& number)
{
    QString timeString;
    QString minuteString;
    QString secondString;
    int a = number / 60;   //得到分钟数
    int b = number % 60; //得到秒数

    //规整一下秒数,使其为两位数
    if(b<10)
    {
        secondString.append("0");
    }
    secondString.append(QString::number(b));

    //规整一下分钟数,使其为两位数
    if(a<10)
    {
        minuteString.append("0");
    }
    minuteString.append(QString::number(a));

    timeString.append(minuteString).append(":").append(secondString);
    return timeString;

}

//事件过滤器
bool CWidget::eventFilter(QObject *target, QEvent *event)
{
    if(m_videoWidget == target)
    {
        if(event->type() == QEvent::MouseButtonPress ||
           event->type() == QEvent::MouseButtonRelease ||
           event->type() == QEvent::MouseButtonDblClick)
        {
            if(m_videoWidget->isFullScreen())
            {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                QPoint point = m_videoWidget->mapToGlobal(mouseEvent->pos());
                if(m_rectBottomWidget.contains(point.x(), point.y()))
                {
                    return true;
                }
            }
        }
    }

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
        { 
            if(keyEvent->key() == Qt::Key_Left)
            {
                slotSignalPressPosition(m_timeSlider->value()-5000);         
            }
            else
            {
                slotSignalPressPosition(m_timeSlider->value()+5000);
            }
            return true;
        }

        if(keyEvent->key() == Qt::Key_Space)
        {
            slotPlayOrPause();
            return true;
        }

        if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
        {
            int volume = m_mediaPlayer->volume();
            if(keyEvent->key() == Qt::Key_Up)
            {
                volume += 3;
                if(volume > 100)  //声音最大可以调到100
                {
                    volume = 100;
                }
                slotPressVolumePosition(volume);
            }
            else
            {
                volume -= 3;
                if(volume < 0)
                {
                    volume = 0;
                }
                slotPressVolumePosition(volume);
            }

            showVolumeToolTip(volume);

            return true;
        }
    }

    if(event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

        int volume = m_mediaPlayer->volume();

        if(wheelEvent->delta() > 0)
        {                                          //当滚轮远离使用者时
            volume += 1;
            if(volume > 100)                       //声音最大可以调到100
            {
                volume = 100;
            }
            slotPressVolumePosition(volume);
        }
        else
        {                                          //当滚轮向使用者方向旋转时
            volume -= 1;
            if(volume < 0)
            {
                volume = 0;
            }
            slotPressVolumePosition(volume);
        }

        showVolumeToolTip(volume);

        return true;
    }

    return QWidget::eventFilter(target, event);
}

void CWidget::showEvent(QShowEvent *event)
{
    this->setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(event);
}

//从路径中得到文件名字
QString CWidget::getNameFromPath(const QString &fileName)
{
    QString name;
    QChar c;
    QString s = fileName.toUtf8().data();
    foreach(c, s)
    {
        if(c =='/')
        {
            name.clear();
            continue;
        }
        name.append(c);
    }
    //name = name.toUtf8().data();     //解决name中文乱码问题
    return name;
}

//使用数据库的数据初始化
void CWidget::getDataFromDatabase()
{
    QString pathDB = QDir::currentPath();
    pathDB += "/media.db";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDB);

    /*QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("media");
    db.setUserName("multin");
    db.setPassword("drama");*/

    if (!db.open())  //如果数据库文件不存在,会在当前路劲下自动创建一个空的名字为media.db数据库文件
    {
        QMessageBox::critical(0, QObject::tr("Database Error"),db.lastError().text());
    }
    else
    {
        QSqlQuery query;
        //query.exec("create database media");
        if(!query.exec("CREATE TABLE mediatable(ind VARCHAR(777), name VARCHAR(777), fileName VARCHAR(777), progress INT64(1))"))
        {
            qDebug()<<query.lastError().text();
        }
        if(!query.exec("CREATE TABLE setting(volume INT64(1), playMode VARCHAR(777))"))
        {
            qDebug()<<query.lastError().text();
        }
    }

    openTable();
}

//得到方向
void CWidget::region(const QPoint &cursorGlobalPoint)
{
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());

    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if(tl.x() + PADDING >= x && tl.x() <= x && tl.y() + PADDING >= y && tl.y() <= y)
    {
        // 左上角
        dir = LEFTTOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
    }
    else if(x >= rb.x() - PADDING && x <= rb.x() && y >= rb.y() - PADDING && y <= rb.y())
    {
        // 右下角
        dir = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(x <= tl.x() + PADDING && x >= tl.x() && y >= rb.y() - PADDING && y <= rb.y()) {
        //左下角
        dir = LEFTBOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(x <= rb.x() && x >= rb.x() - PADDING && y >= tl.y() && y <= tl.y() + PADDING) {
        // 右上角
        dir = RIGHTTOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(x <= tl.x() + PADDING && x >= tl.x()) {
        // 左边
        dir = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if( x <= rb.x() && x >= rb.x() - PADDING) {
        // 右边
        dir = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(y >= tl.y() && y <= tl.y() + PADDING){
        // 上边
        dir = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if(y <= rb.y() && y >= rb.y() - PADDING) {
        // 下边
        dir = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else
    {
        // 默认
        dir = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

//过滤序号
QString CWidget::removeIndexFromAppend( QString name)
{
    QString temp = name.right(2);
    temp=temp.left(1);
    if(temp=="0")
    {
        return (name.remove(name.size()-3, 3));
    }
    else
    {
        return name;
    }
}

void CWidget::enterEvent(QEvent *event)
{
    qDebug()<<"Widget::鼠标进入";
    QWidget::enterEvent(event);
}

void CWidget::keyPressEvent(QKeyEvent *event)
{
    int keyMsg = event->key();

    if(keyMsg == Qt::Key_Space)
    {
        slotVideoWidgetKeySpace();
        return;
    }

    QWidget::keyPressEvent(event);
}

void CWidget::paintEvent(QPaintEvent *event)
{    
    QWidget::paintEvent(event);
}

void CWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void CWidget::leaveEvent(QEvent * event)
{
    //m_hideButtonBase->hide();
    qDebug()<<"Widget::鼠标离开::m_hideButton->hide();";
    QWidget::leaveEvent(event);
}

void CWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseGlobalPos = event->globalPos();
    QPoint gloPoint = event->globalPos();
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());

    if(!m_bIsLeftPressDown)  //如果左键没有按下，则更新鼠标相对于Widget的位置信息
    {
        this->region(gloPoint);
    }
    else
    {
        if(dir != NONE)
        {
            QRect rMove(tl, rb);

            switch(dir) {   //处于边框拖动的状态
            case LEFT:
              if(rb.x() - gloPoint.x() <= this->minimumWidth())
                   rMove.setX(tl.x());
              else
                  rMove.setX(gloPoint.x());
             //   rMove.setWidth(rb.x()-gloPoint.x());    //以右边界为标准变化， 左边不变，所以不能用
                break;
            case RIGHT:
                rMove.setWidth(gloPoint.x() - tl.x());

                break;
            case UP:
                if(rb.y() - gloPoint.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(gloPoint.y());

                break;
            case DOWN:
                rMove.setHeight(gloPoint.y() - tl.y());                   //以上边界为标准变化， 左边不变，所以不能用
                break;
            case LEFTTOP:
                if(rb.x() - gloPoint.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(gloPoint.x());
                if(rb.y() - gloPoint.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(gloPoint.y());
                break;
            case RIGHTTOP:
               rMove.setWidth(gloPoint.x() - tl.x());
               if(rb.y() - gloPoint.y() <= this->minimumHeight())
                   rMove.setY(tl.y());
               else
               rMove.setY(gloPoint.y());    //May change the height, but will never change the bottom edge of the rectangle.

                break;
            case LEFTBOTTOM:
                if(rb.x() - gloPoint.x() <= this->minimumWidth())
                     rMove.setX(tl.x());
                else
                     rMove.setX(gloPoint.x());
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            case RIGHTBOTTOM:
                rMove.setWidth(gloPoint.x() - tl.x());
                rMove.setHeight(gloPoint.y() - tl.y());

                break;
            default:
                break;
            }
            this->setGeometry(rMove);
        }
        else if(isMaximized())  //处于最大化
        {
            double percent= static_cast<double>(event->globalPos().x() ) / static_cast<double>(m_screenWidth);
            slotSetMaxButton();
            double repaint = event->globalPos().x() - normalRect.width() * percent;    //重绘窗口的起始X坐标
            if( gloPoint.x() < (normalRect.width() - 100) )  // 靠在左上边窗口
            {
                this->setGeometry(0, 0, normalRect.width(), normalRect.height());
            }
            else if (m_screenWidth - normalRect.width()+100<gloPoint.x() )  //靠在右上边窗口
            {
                this->setGeometry(m_screenWidth - normalRect.width(),  0,  normalRect.width(), normalRect.height() );
            }
            else     //正常情况下居中显示
            {
                this->setGeometry(repaint, 0, normalRect.width(), normalRect.height() );
            }
            dragPosition = event->globalPos() - this->frameGeometry().topLeft();   //紧接着设置新的移动位置
        }
        else
        {
            move(event->globalPos()-dragPosition );
        }

        normalRect = this->rect();
        refreshLayout(this->rect());

        qDebug()<<"normalRect: "<<normalRect;

        topWidgetLayout();

        videoWidgetLayout();

        m_listWidget->setGeometry(m_rectListWidget);

        bottomWidgetLayout();

    }
    QWidget::mouseMoveEvent(event);
}

void CWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bIsLeftPressDown = false;
        if(dir != NONE)
        {
            this->releaseMouse();
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void CWidget::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::LeftButton:
        m_bIsLeftPressDown = true;
        if(dir != NONE)
        {
            this->mouseGrabber();
        }
        else
        {
            dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        }
        break;
    case Qt::RightButton:

        break;
    default:

        break;
    }
}

//鼠标双击事件
void CWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void CWidget::slotCloseWidget()
{
    if(QMediaPlayer::StoppedState != m_mediaPlayer->state())
    {
        m_mediaPlayer->stop();
    }

    //updateToDataBase();

    int pointX =this->pos().x();
    int pointY =this->pos().y();
    int height = normalRect.height();
    int tmp = height;
    this->setMinimumSize(0,0);
    for(int i=0; i < height/2;++i)
    {
        pointY += 10;
        tmp -= 20;
        this->setGeometry(pointX, pointY, normalRect.width(), tmp);
        m_videoWidgetBase->setGeometry(0,  50, normalRect.width()-200,   tmp-49);
        m_videoWidget->setGeometry(0, 0, m_videoWidgetBase->size().width(), m_videoWidgetBase->size().height());
        m_mainLabel->setGeometry(0, 0, m_videoWidgetBase->size().width(), m_videoWidgetBase->size().height());
    }
    this->close();
}

//显示错误
void CWidget::slotHandleError()
{
    qDebug()<<"Error:  " << m_mediaPlayer->errorString();
}

//设置隐藏按钮
void CWidget::slotHideButton()
{
    if(m_videoWidget->isFullScreen())
    {
        m_rightWidget->isHidden() ? m_rightWidget->show() : m_rightWidget->hide();
        setState(true);
    }
    else
    {
        m_rightWidgetBase->isHidden() ? m_rightWidgetBase->show() : m_rightWidgetBase->hide();
        setState(false);
    }
}

//鼠标进入目标区域
void CWidget::slotInBottomRegion()
{
    if(m_videoWidget->getBottomWidgetVisible())
    {
        if(!m_listWidget->isVisible())
        {
            m_bottomWidget->show();
            qDebug()<<"slotInBottomRegion():: m_bottomWidget->show();";
        }
    }
}

void CWidget::slotInRightRegion()
{
    if(m_videoWidget->getHideBottonVisible())
    {
        m_hideButtonBase->show();
        qDebug()<<"slotInRightRegion()::m_hideButton->show();";
    }
}

//使用m_videoWidget移动窗体
void CWidget::slotSignalMovePoint(QPoint point)
{
    if(this->isMaximized())
    {
        QPoint posi = this->mapToGlobal(this->cursor().pos());
        double percent= static_cast<double>(posi.x() ) / static_cast<double>(m_screenWidth);
        slotSetMaxButton();
        double repaint = posi.x() - normalRect.width() * percent;    //重绘窗口的起始X坐标
        if( posi.x() < (normalRect.width() - 100) )  // 靠在左上边窗口
        {
            this->setGeometry(0, 0, normalRect.width(), normalRect.height());
        }
        else if (m_screenWidth - normalRect.width()+100<posi.x() )  //靠在右上边窗口
        {
            this->setGeometry(m_screenWidth - normalRect.width(),  0,  normalRect.width(), normalRect.height() );
        }
        else     //正常情况下居中显示
        {
            this->setGeometry(repaint, 0, normalRect.width(), normalRect.height() );
        }
    }
    else
    {
        this->move(point+Position-QPoint(0,0));
        if(NULL != m_volumeToolTip)
        {
            m_volumeToolTip->move(m_videoWidget->mapToGlobal(m_volumeToolTipPos));
        }
    }
}

//鼠标出去目标区域
void CWidget::slotOutBottomRegion()
{
    m_bottomWidgetStartTime = QTime::currentTime();
    m_outRegionTimer->start(10);
}

void CWidget::slotOutRightRegion()
{
    m_hideBottonStartTime = QTime::currentTime();
    m_outRightRegionTimer->start(10);
}

//设置是否隐藏bottomWidget
void CWidget::slotOutBottomRegionTimer()
{
    int msecsCount = m_bottomWidgetStartTime.msecsTo(QTime::currentTime());
    if(m_videoWidget->isFullScreen() && !m_videoWidget->getBottomWidgetVisible() && msecsCount > 2000)
    {
        //m_bottomWidget->hide();
        m_outRegionTimer->stop();
        qDebug()<<"slotOutBottomRegionTimer():: m_bottomWidget->hide();";
    }
}

//设置是否隐藏hideButton
void CWidget::slotOutRightRegionTimer()
{
    int msecsCount = m_hideBottonStartTime.msecsTo(QTime::currentTime());
    if(!m_videoWidget->getHideBottonVisible() && msecsCount > 2000)
    {
        m_hideButtonBase->hide();
        m_outRightRegionTimer->stop();
        qDebug()<<"slotOutRightRegionTimer()::m_hideButton->hide();";
    }
}

//响应最大化按钮
void CWidget::slotSetMaxButton()
{
    QRect client = this->rect();

    if(m_screenWidth == client.width() && m_screenHeight == client.height())  //处于最大化时
    {
        client.setRect(0, 0, getWindowIniWidth(), getWindowIniHeight());
        this->showNormal();
    }
    else
    {
        client.setRect(0, 0, m_screenWidth, m_screenHeight);
        this->showMaximized();
    }

    refreshLayout(client);

    videoWidgetLayout();

    rightWidgetLayout();

    topWidgetLayout();
    bottomWidgetLayout();
}

//双击列表项更换视频
void CWidget::slotListWidgetItemDoubleClick(int currentIndex)
{
    if(0 > currentIndex || currentIndex >= m_mediaPlaylist->mediaCount())
    {
        return;
    }

    m_mediaPlaylist->setCurrentIndex(currentIndex);
    //m_mediaPlayer->setPosition(m_vectorProgress.at(currentIndex));
    m_mediaPlayer->play();


//    qint64 pos = m_vectorProgress.at(currentIndex);
//    slotSignalPressPosition(pos);


}

void CWidget::slotCureentItemDoubleClick(QListWidgetItem *current)
{
    /*int itemRow = index.row();

    m_mediaPlaylist->setCurrentIndex(itemRow);
    m_mediaPlayer->play();*/
    qDebug()<<"CWidget::slotcureentItemDoubleClick(QListWidgetItem *current)";
}

//删除列表项
void CWidget::slotListWidgetItemDeleted(int n)
{
    int mediaCount = m_mediaPlaylist->mediaCount();
    if(n >= mediaCount || n < 0)
    {
        return;
    }

    if(mediaCount < 0)
    {
        m_titleLabel->clear();
        m_playButton->setEnabled(false);
        m_mediaPlayer->stop();
    }

    QListWidgetItem* item = m_listWidget->takeItem(n);
    delete item;
    m_mediaPlaylist->removeMedia(n);
    //m_vectorProgress.remove(n);
    updateToDataBase();
}

//清空列表
void CWidget::slotListWidgetClearAll()
{
    m_mediaPlayer->stop();
    qDebug()<<"m_mediaPlayer->stop() ";
    m_listWidget->clear();
    m_mediaPlaylist->clear();
    m_titleLabel->clear();
    m_vectorProgress.clear();
}

//清空失效记录
void CWidget::slotListWidgetClearValidItem()
{
    int mediaCount = m_mediaPlaylist->mediaCount();
    for(int i = 0; i < mediaCount; i++)
    {
        QFileInfo fileInfo(m_mediaPlaylist->media(i).canonicalUrl().toLocalFile());
        if(fileInfo.exists())
        {
            continue;
        }
        else
        {
            m_mediaPlaylist->removeMedia(i);
            QListWidgetItem* item = m_listWidget->takeItem(i);
            delete item;
        }
    }

    if(mediaCount != m_mediaPlaylist->mediaCount())
    {
        updateToDataBase();
    }
}

void CWidget::slotListWidgetOpenDirectory(int n)
{
    if(n < 0 || n >= m_mediaPlaylist->mediaCount())
    {
        return;
    }

    QString filePath = m_mediaPlaylist->media(n).canonicalUrl().toLocalFile();
    QFileInfo info(filePath);

    QString path=info.absolutePath();                   //获取程序当前目录
    qDebug()<<"path:  "<<path.replace("/","\\");        //将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
    QProcess::startDetached("explorer "+path);          //打开上面获取的目录
}

void CWidget::slotListWidgetAddMediaFile(QStringList strList)
{
    int strCount = strList.count();

    if(strCount < 1)
    {
        return;
    }

    for(int i = 0; i < strCount; i++)
    {
        QString filePath = strList.at(i);

        addFile(filePath);
    }

//    if(m_mediaPlayer->state() != QMediaPlayer::PlayingState)
//    {
//        m_mediaPlaylist->setCurrentIndex(0);
//        m_listWidget->setCurrentRow(0);
//    }
}

//打开文件
void CWidget::slotOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "open file", QDir::currentPath());
    addFile(fileName);
}

//根据当前状态设置播放按钮的图标
void CWidget::slotStateChanged(QMediaPlayer::State state)
{
    switch(state)
    {
    case QMediaPlayer::PlayingState:
    {
        //m_mainLabel->hide();       //容易黑屏闪烁
        m_playButton->setStyleSheet("QPushButton{image:url(:/images/images/pause.png);"
                                                 "border-radius:25px; "
                                                 "background-color: transparent}"
                                    "QPushButton::hover{background-color: rgb(90,90,90);"
                                                        "image:url(:/images/images/pauseHovered.png)}");
        break;
    }
    case QMediaPlayer::PausedState:
    {
        m_playButton->setStyleSheet("QPushButton{image:url(:/images/images/play.png);"
                                                 "border-radius:25px; "
                                                 "background-color: transparent}"
                                    "QPushButton::hover{background-color: rgb(90,90,90); "
                                                        "image:url(:/images/images/playHovered.png)}");
        QScreen *screen =  QApplication::primaryScreen();
        QPixmap pixmap= screen->grabWindow(m_videoWidget->winId());
        m_mainLabel->setPixmap(pixmap);
        m_mainLabel->setGeometry(0, 0, m_videoWidgetBase->size().width(), m_videoWidgetBase->size().height());
        m_mainLabel->show();      //当视频播放时，窗口重绘会覆盖mainLabel,所以可以不用隐藏
        break;
    }
    default:
    {
        m_playButton->setStyleSheet("QPushButton{image:url(:/images/images/play.png);"
                                                 "border-radius:25px;"
                                                 "background-color: transparent}"
                                    "QPushButton::hover{background-color: rgb(90,90,90); "
                                                        "image:url(:/images/images/playHovered.png)}");

        break;
    }
    }
}

//响应播放暂停按钮
void CWidget::slotPlayOrPause()
{
    switch(m_mediaPlayer->state())
    {
    case QMediaPlayer::PlayingState:
    {
        m_mediaPlayer->pause();
        m_playButton->setToolTip("播放");
        break;
    }
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
    {
        m_mediaPlayer->play();
        m_playButton->setToolTip("暂停");
        break;
    }
    }
}

//下一曲
void CWidget::slotNextVideo()
{
    if(0 < m_mediaPlaylist->mediaCount())
    {
        int nextIndex = m_mediaPlaylist->currentIndex();
        if(nextIndex == m_mediaPlaylist->mediaCount()-1)
        {
            nextIndex = 0;
        }
        else
        {
            nextIndex += 1;
        }
        m_mediaPlaylist->setCurrentIndex(nextIndex);
        m_mediaPlayer->play();
        //m_mediaPlayer->setPosition(m_vectorProgress.at(nextIndex));

        //qint64 pos = m_vectorProgress.at(nextIndex);
        //slotSignalPressPosition(pos);
    }
}

//上一曲
void CWidget::slotPreviousVideo()
{
    if(0 < m_mediaPlaylist->mediaCount())
    {
        int previousIndex = m_mediaPlaylist->currentIndex();

        if(previousIndex == 0)
        {
            previousIndex = m_mediaPlaylist->mediaCount()-1;
        }
        else
        {
            previousIndex -= 1;
        }
        m_mediaPlaylist->setCurrentIndex(previousIndex);
        m_mediaPlayer->play();
        //m_mediaPlayer->setPosition(m_vectorProgress.at(previousIndex));

        //qint64 pos = m_vectorProgress.at(previousIndex);
        //slotSignalPressPosition(pos);
    }
}

//停止视频
void CWidget::slotStopVideo()
{
    if(m_mediaPlayer->state() != QMediaPlayer::StoppedState)
    {
        m_mediaPlayer->stop();
        m_mainLabel->clear();
        m_mainLabel->show();
    }
}

//设置主进度条的值
void CWidget::slotSignalPressPosition(qint64 position)
{
    if(QMediaPlayer::StoppedState != m_mediaPlayer->state())
    {
        m_mediaPlayer->setPosition(position);
        //m_timeSlider->setValue(position);
    }
}

void CWidget::slotMouseMovePosition(qint64 position)
{
    if(QMediaPlayer::StoppedState != m_mediaPlayer->state())
    {
        int secs = position/1000;   //秒

        QString strToolTip = timeFormat(secs);

        m_timeSlider->setToolTipText(strToolTip);
    }
    else
    {
        if(m_timeSlider->getToolTip())
        {
            m_timeSlider->getToolTip()->close();
        }
    }
}

//设置声音进度条的值
void CWidget::slotPressVolumePosition(qint64 position)
{
    showVolumeToolTip(position);
    m_mediaPlayer->setVolume(position);
    if(position >= 0 && position <= 100)
    {
        m_volumeSlider->setValue(position);
    }

    bool bIsMute = m_volumeSlider->value() ? false : true;
    isMuteVisible(bIsMute);

    m_volumePos = position;
}

//
void CWidget::slotVideoWidgetDropedList(QList<QUrl> urls)
{
    QString fileName;
    for(int i=0;i<urls.size();++i)
    {
        fileName = urls.at(i).toLocalFile();
        qDebug()<<fileName;
        addFile(fileName);
    }
}

//再m_videoWidget上按下鼠标左键
void CWidget::slotVideoWidgetMouseLeftDown()
{
    Position = this->pos();
}

//双击全屏或者ESC退出全屏
void CWidget::slotVideoWidgetDoubleClickedOrKeyEscape(bool bIsDoubleClicked)
{  
    if(NULL != m_volumeToolTip)
    {
        if(!m_volumeToolTip->isHidden())
        {
            m_volumeToolTip->hide();
        }
    }

    if(bIsDoubleClicked)  //鼠标双击
    {
        setFullScreen(!m_videoWidget->isFullScreen());
    }
    else        //键盘按键ESC键按下
    {     
        setFullScreen(false);
    }
}

void CWidget::slotVideoWidgetKeySpace()
{
    slotPlayOrPause();
}

//
void CWidget::slotVideoWidgetActionCapture()
{
    m_mediaPlayer->pause();
    int width = QApplication::desktop()->availableGeometry().width();
    int height = QApplication::desktop()->availableGeometry().height();
    QScreen *screen =  QApplication::primaryScreen();
    QPixmap pixmap= screen->grabWindow(m_videoWidget->winId());
    CCaptureDialog* dialog = new CCaptureDialog();
    dialog->setGroupBoxSize(m_videoWidget->size());
    dialog->setPixmap(pixmap);
    dialog->setGeometry((width-dialog->getNormalRect().width())/2, (height-dialog->getNormalRect().height())/2, dialog->getNormalRect().width(), dialog->getNormalRect().height());
    dialog->show();

}

void CWidget::slotVideoWidgetActionInfo()
{
//    QString fileName = vector.at(playingIndex).first.second;
//    QFileInfo info(fileName);
//    qDebug()<<"fileName:  "<<fileName;
//    qDebug()<<"suffix:  "<<info.suffix();
//    qDebug()<<info.size()/1024/1024;
//    QMediaResource resource(QUrl::fromLocalFile(fileName));
//    resource.setVideoCodec("video/avc");
//    qDebug()<<"is null : "<<resource.isNull();
//    qDebug()<<"language : "<<resource.language();
//    qDebug()<<"videoCodec:  "<<resource.videoCodec();
//    qDebug()<<"resolution:  "<<resource.resolution();
//    qDebug()<<"video size:  "<<resource.dataSize();
//    QMediaRecorder recorder(m_mediaPlayer);
//    //qDebug()<<"videoSettings:  "<<recorder.videoSettings.codec();
//    qDebug()<<""<<recorder.videoCodecDescription("h.261");


    //QSize size = m_mediaPlayer->currentMedia().canonicalResource().resolution();
    //QSize size = m_mediaPlayer->currentMedia().resources().at(0).resolution();
//    qDebug()<<m_mediaPlayer->currentMedia().canonicalResource().url();
//    qDebug()<<"is null:  "<<m_mediaPlayer->currentMedia().isNull();
//    qDebug()<<"audioBitRate:   "<<m_mediaPlayer->currentMedia().canonicalResource().audioBitRate();
//    qDebug()<<"audioCodec:  "<<m_mediaPlayer->currentMedia().canonicalResource().audioCodec();
//    qDebug()<<"dataSize:  "<<m_mediaPlayer->currentMedia().canonicalResource().dataSize();
//    qDebug()<<"videoBitRate:  "<<m_mediaPlayer->currentMedia().canonicalResource().videoBitRate();

}

//播放视频时mediaPlayer的响应函数
void CWidget::slotPositionChanged(qint64 value)
{
    //当文件播放位置变化，更新进度显示
    if(m_timeSlider->isSliderDown())//正处于手动调整状态，不处理
    {
        return;
    }
    m_timeSlider->setSliderPosition(value);
    int secs = value/1000;//秒

    m_positionTime = timeFormat(secs);

    m_timeLabel->setText(m_positionTime+" / "+m_durationTime);


    //int index = m_mediaPlaylist->currentIndex();
//    int testValue = m_vectorProgress.at(index);
    //m_vectorProgress.replace(index, value);//保存当前的播放进度
//    testValue = m_vectorProgress.at(index);
//    qDebug()<<index<<testValue;
}

//设置当前视频播放的总时间
void CWidget::slotDurationChanged(qint64 value)
{
    //文件时长变化，更新进度显示
    m_timeSlider->setMaximum(value);
    int secs = value/1000;//秒

    m_durationTime = timeFormat(secs);

    m_timeLabel->setText(m_positionTime+" / "+m_durationTime);
}

//使m_mediaPlayList更新database
void CWidget::updateToDataBase()
{
    //更新数据库表mediatable
    QSqlTableModel mediaTableModel;
    mediaTableModel.setTable("mediatable");
    mediaTableModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
    if(!mediaTableModel.select())
    {
        QMessageBox::critical(this, "error", "打开数据表mediatable错误,错误信息\n"
                              +mediaTableModel.lastError().text(),
                              QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    if(mediaTableModel.rowCount())
    {
        mediaTableModel.removeRows(0, mediaTableModel.rowCount());
        mediaTableModel.submitAll();
    }

    int mediaCount = m_mediaPlaylist->mediaCount();
    if(0 != mediaCount)
    {
        QSqlRecord recordMediaTable = mediaTableModel.record();
        for(int i = 0; i < mediaCount; i++)
        {
            recordMediaTable.setValue(0, QString::number(i));
            recordMediaTable.setValue(1, m_mediaPlaylist->media(i).canonicalUrl().fileName());
            recordMediaTable.setValue(2, m_mediaPlaylist->media(i).canonicalUrl().toLocalFile());

            qint64 value = m_vectorProgress.at(i);
            recordMediaTable.setValue(3, value);

            mediaTableModel.insertRecord(i, recordMediaTable);
        }
        mediaTableModel.submitAll();
    }

    //更新数据库表setting
    QSqlTableModel settingModel;
    settingModel.setTable("setting");
    settingModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
    if(!settingModel.select())
    {
        QMessageBox::critical(this, "error", "打开数据表setting错误,错误信息\n"
                              +settingModel.lastError().text(),
                              QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    if(settingModel.rowCount())
    {
        settingModel.setData(settingModel.index(0, 0), m_volumePos);
        settingModel.setData(settingModel.index(0, 1), m_playingMode);
        settingModel.submitAll();
    }
    else
    {
        settingModel.removeRows(0, 1);
        settingModel.submitAll();

        QSqlRecord recordSetting = settingModel.record();
        recordSetting.setValue(0, m_volumePos);
        recordSetting.setValue(1, m_playingMode);
        settingModel.insertRecord(0, recordSetting);
        settingModel.submitAll();
    }
}

void CWidget::initTopWidget()
{
    //用于放顶部控件
    m_topWidget = new CTopWidget(this);
    m_topWidget->setGeometry(m_rectTopWidget);
    m_topWidget->setStyleSheet("QWidget{border-top-left-radius:0px;"
                                        "border-top-right-radius:0px;"
                                        "background-color:rgb(38,38,38)}");
    m_topWidget->setCursor(QCursor(Qt::ArrowCursor));
    m_topWidget->installEventFilter(this);

    connect(m_topWidget, SIGNAL(signalMovePoint(QPoint)), this, SLOT(slotSignalMovePoint(QPoint)));
    connect(m_topWidget, SIGNAL(signalLeftPressDown()), this, SLOT(slotVideoWidgetMouseLeftDown()));
    connect(m_topWidget, SIGNAL(signalDoubleClicked()),  this, SLOT(slotBottomWidgetDoubleClicked()));

    //关闭按钮
    m_closeButton = new QPushButton(m_topWidget);
    m_closeButton->setFixedSize(30,20);
    m_closeButton->setToolTip("关闭");
    m_closeButton->setStyleSheet("QPushButton{image:url(:/images/images/close.png);"
                                             "border-radius:8px; "
                                             "background-color:rgb(60,65,68);}"
                                             "QPushButton:hover{image:url(:/images/images/closeHovered.png);"
                                                                "background-color:rgb(90,90,90);}");
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(slotCloseWidget()));
    m_closeButton->installEventFilter(this);

    //最大化/还原按钮
    m_maximumButton = new QPushButton(m_topWidget);
    m_maximumButton->setFixedSize(30,20);
    m_maximumButton->setToolTip("最大化/还原");
    m_maximumButton->setStyleSheet("QPushButton{image:url(:/images/images/maximize.png); "
                                              "border-radius:8px; "
                                              "background-color:rgb(60,65,68);}"
                                              "QPushButton:hover{image:url(:/images/images/maximizeHovered.png); "
                                              "background-color:rgb(90,90,90);}");
    connect(m_maximumButton, SIGNAL(clicked()), this, SLOT(slotSetMaxButton()));
    m_maximumButton->installEventFilter(this);

    //最小化按钮
    m_minimumButton = new QPushButton(m_topWidget);
    m_minimumButton->setFixedSize(30,20);
    m_minimumButton->setToolTip("最小化");
    //QString name = QDir::currentPath();
    m_minimumButton->setStyleSheet("QPushButton{image:url(:/images/images/minimize.png); "
                                                "border-radius:8px; "
                                                "background-color:rgb(60,65,68);}"
                                   "QPushButton:hover{image:url(:/images/images/minimizeHovered.png); "
                                                      "background-color:rgb(90,90,90);}");
    connect(m_minimumButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    m_minimumButton->installEventFilter(this);

    //左上角的菜单
    m_menuButton = new QPushButton(m_topWidget);
    m_menuButton->setStyleSheet("QPushButton{image:url(:/images/images/appIcon.png);"
                                           "border-radius:15px; "
                                           "border: none; "
                                           "background-color:rgb(120,125,128);}"
                                           "QPushButton:hover{background-color:rgb(200,200,201);} "
                                           "QPushButton:checked{background-color:rgb(0,0,1);}"
                                           "QPushButton::menu-indicator{image:None;}");
    //connect(m_menuButton, &QPushButton::clicked, this, &Widget::slotMenuButton);
    m_menuButton->installEventFilter(this);

    m_menuButton2 = new QPushButton(m_topWidget);
    m_menuButton2->setText("千年老妖");
    m_menuButton2->setStyleSheet("QPushButton{border:none; "
                                            "color:rgb(224,224,224); "
                                            "font-size:21px; "
                                            "font-family: Comic Sans MS;}"
                               "QPushButton::menu-indicator{image:none;}");
    //connect(menuButton2, &QPushButton::clicked, this, &Widget::slotMenuButton);
    m_menuButton2->installEventFilter(this);

    //标题栏
    m_titleLabel = new QLabel(m_topWidget);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel{color:rgb(200,200,200); "
                                      "background-color:transparent;}");
    m_titleLabel->installEventFilter(this);

    m_menu = new QMenu;
    m_openAction = m_menu->addAction("打开");
    m_menu->setStyleSheet("QMenu{border:none;}"
                                 "QMenu::item{border:none;"
                                 "font-size:13px;"
                                 "padding-left:20px;"
                                 "background-color:white;"
                                 "height:25px; "
                                 "width:50px;}"
                           "QMenu::item:selected{ background-color:rgb(140,140,240)}");

    topWidgetLayout();

}

void CWidget::initBottomWidget()
{
    //用于放置底部的所有按钮
    m_bottomWidgetBase = new QWidget(this);
    m_bottomWidgetBase->setGeometry(m_rectBottomWidget);
    m_bottomWidgetBase->setStyleSheet("QWidget{background-color:rgba(0,0,255,0);}");

    m_bottomWidget = new CBottomWidget(m_bottomWidgetBase);
    m_bottomWidget->setVisible(true);
    m_bottomWidget->setBackgroundColor(QColor(0, 0, 255, 0));
    m_bottomWidget->setWindowFlags(Qt::FramelessWindowHint);
    m_bottomWidget->setAttribute(Qt::WA_TranslucentBackground);

    m_bottomWidget->setCursor(QCursor(Qt::ArrowCursor));
    connect(m_bottomWidget, SIGNAL(signalMovePoint(QPoint)), this, SLOT(slotSignalMovePoint(QPoint)));
    connect(m_bottomWidget, SIGNAL(signalLeftPressDown()), this, SLOT(slotVideoWidgetMouseLeftDown()));
    m_bottomWidget->installEventFilter(this);

    //时间滑动条
    m_timeSlider = new CMySlider(Qt::Horizontal, m_bottomWidget);
    m_timeSlider->setToolTipVisible(true);
    m_timeSlider->setRange(0, 0);
    connect(m_timeSlider, SIGNAL(signalPressPosition(qint64)), this, SLOT(slotSignalPressPosition(qint64 )));
    connect(m_timeSlider, SIGNAL(signalMouseMovePosition(qint64)), this, SLOT(slotMouseMovePosition(qint64 )));
    m_timeSlider->installEventFilter(this);

    //停止按钮
    m_stopButton = new QPushButton(m_bottomWidget);
    m_stopButton->setToolTip("停止");
    m_stopButton->setStyleSheet("QPushButton{image:url(:/images/images/stop.png);"
                                             "border-radius:20px;"
                                             "background-color: transparent}"
                                "QPushButton::hover{background-color: rgb(90,90,90);"
                                                    "image:url(:/images/images/stopHovered.png);}");
    connect(m_stopButton, SIGNAL(clicked(bool)), this, SLOT(slotStopVideo()));
    m_stopButton->installEventFilter(this);

    //上一曲按钮
    m_previousVideoButton =  new QPushButton(m_bottomWidget);
    m_previousVideoButton->setToolTip("上一个");
    m_previousVideoButton->setStyleSheet("QPushButton{image:url(:/images/images/backward.png);"
                                                      "border-radius:20px; "
                                                      "background-color: transparent}"
                                         "QPushButton:hover{background-color: rgb(90,90,90); "
                                                            "image:url(:/images/images/backwardHovered.png)}");
    connect(m_previousVideoButton, SIGNAL(clicked(bool)), this, SLOT(slotPreviousVideo()));
    m_previousVideoButton->installEventFilter(this);

    //播放按钮
    m_playButton = new QPushButton(m_bottomWidget);
    m_playButton->setToolTip("播放");
    m_playButton->setStyleSheet("QPushButton{image:url(:/images/images/play.png);"
                                             "border-radius:25px; "
                                             "background-color: transparent}"
                                "QPushButton::hover{background-color: rgb(90,90,90); "
                                                    "image:url(:/images/images/playHovered.png)}");
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(slotPlayOrPause()));
    m_playButton->installEventFilter(this);

    //下一曲按钮
    m_nextVideoButton = new QPushButton(m_bottomWidget);
    m_nextVideoButton->setToolTip("下一个");
    m_nextVideoButton->setStyleSheet("QPushButton{image:url(:/images/images/forward.png);"
                                                  "border-radius:20px; "
                                                  "background-color: transparent}"
                                     "QPushButton::hover{background-color: rgb(90,90,90);"
                                                         "image:url(:/images/images/forwardHovered.png)}");
    connect(m_nextVideoButton, SIGNAL(clicked(bool)), this, SLOT(slotNextVideo()));
    m_nextVideoButton->installEventFilter(this);

    //显示时间的标签
    m_timeLabel = new QLabel(m_bottomWidget);
    m_timeLabel->setStyleSheet("QLabel{color:rgb(250,250,250);"
                                       "background-color:transparent;}");
    m_timeLabel->installEventFilter(this);

    //音量按钮
    m_volumeButton = new QPushButton(m_bottomWidget);
    m_volumeButton->setToolTip("静音");
    m_volumeButton->setStyleSheet("QPushButton{image:url(:/images/images/music-voice.png);"
                                               "border-radius:15px; "
                                               "background-color: transparent}"
                                  "QPushButton::hover{image:url(:/images/images/music-voice-hover.png);"
                                                      "border-color:1px black; "
                                                      "background-color:rgb(90,90,90);}");
    connect(m_volumeButton, SIGNAL(clicked()), this, SLOT(slotVolumeButtonCliced()));
    m_volumeButton->installEventFilter(this);

    //声音滑动条
    m_volumeSlider = new CMySlider(Qt::Horizontal, m_bottomWidget);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setCursor(QCursor(Qt::PointingHandCursor));
   /*m_volumeSlider->setStyleSheet("QSlider{ background-color: transparent;"
                                            "border:0px solid rgb(25, 38, 58);}"
                                  "QSlider::add-page:vertical{ background-color: rgb(37, 168, 198);"
                                                               "width: 4px;}"
                                  "QSlider::sub-page:vertical{ background-color: rgb(87, 97, 106);"
                                                               "width:4px;}"
                                  "QSlider::groove:vertical{ background:transparent;"
                                                             "width:4px; }"
                                  "QSlider::handle:vertical{ background-color:rgb(222,222,222); "
                                                             "height: 10px; "
                                                             "width:10px;"
                                                             "margin:-1 -3px;"
                                                             "border-radius:5px;}"
                        );*/

    m_volumeSlider->setStyleSheet(
                                  "QSlider::add-page:Horizontal{ background-color: rgba(76, 76, 76, 255);"
                                                                "height:4px;}"
                                  "QSlider::sub-page:Horizontal{ background-color: rgb(51, 153, 255);"
                                                                 "height:4px;}"
                                  "QSlider::groove:Horizontal{ background-color: rgba(0, 0, 0, 0);"
                                                               "height:5px;}"
                                  "QSlider::handle:Horizontal{ height:13px;"
                                                               "width:13px;"
                                                               "border-image: url(:/images/images/circle3.png);"
                                                               "margin-left: 0px;"
                                                               "margin-top: -4px;"
                                                               "margin-right: 0px;"
                                                               "margin-bottom: -4px;}"
                                  );

    connect(m_volumeSlider, SIGNAL(signalPressPosition(qint64)), this, SLOT(slotPressVolumePosition(qint64)));
    m_volumeSlider->installEventFilter(this);

    //播放模式按钮
    m_playModeButton = new QPushButton(m_bottomWidget);
    m_playModeButton->setToolTip("播放模式");
    m_playModeButton->setStyleSheet("QPushButton{image:url(:/images/images/itemOnce.png);"
                                                 "border-radius:15px;"
                                                 "background-color: transparent}"
                                    "QPushButton::hover{ background-color:rgb(160,160,245);}"
                                    "QPushButton::menu-indicator{image:none;}");

    m_playModeButton->installEventFilter(this);

    //播放模式菜单
    m_menuPlayMode = new QMenu;
    m_menuPlayMode->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_menuPlayMode->setAttribute(Qt::WA_TranslucentBackground);

    m_actSinglePlay = m_menuPlayMode->addAction("单个播放");
    m_actSinglePlay->setCheckable(true);
    m_actSinglePlay->setChecked(true);

    m_actSingleCycle = m_menuPlayMode->addAction("单个循环");
    m_actSingleCycle->setCheckable(true);

    m_actOrderPlay = m_menuPlayMode->addAction("顺序播放");
    m_actOrderPlay->setCheckable(true);

    m_actLoopPlay = m_menuPlayMode->addAction("列表循环");
    m_actLoopPlay->setCheckable(true);

    m_actShufflePlay = m_menuPlayMode->addAction("随机播放");
    m_actShufflePlay->setCheckable(true);

    m_menuPlayMode->setStyleSheet("QMenu{border:none;"
                                         "border-radius: 8px;}"
                                  "QMenu::item{border:none;"
                                               "border-radius: 10px;"
                                               "font-size:16px;"
                                               "padding-left:25px;"
                                               "background-color:white;"
                                               "height:40px; width:90px;}"
                                  "QMenu::item:selected{ background-color:rgb(140,140,240)}");

    connect(m_actSinglePlay, SIGNAL(triggered()), this, SLOT(slotPlayMode()));
    connect(m_actSingleCycle, SIGNAL(triggered()), this, SLOT(slotPlayMode()));
    connect(m_actOrderPlay, SIGNAL(triggered()), this, SLOT(slotPlayMode()));
    connect(m_actLoopPlay, SIGNAL(triggered()), this, SLOT(slotPlayMode()));
    connect(m_actShufflePlay, SIGNAL(triggered()), this, SLOT(slotPlayMode()));
    m_playModeButton->setMenu(m_menuPlayMode);

    //全屏模式按钮
    m_fullScreenButton = new QPushButton(m_bottomWidget);
    m_fullScreenButton->setToolTip("全屏");
    m_fullScreenButton->setStyleSheet("QPushButton{image:url(:/images/images/fullScreen_alt.png);"
                                                   "border-radius:15px;"
                                                   "background-color: transparent}"
                                      "QPushButton::hover{ background-color:rgb(160,160,245);}");
    connect(m_fullScreenButton, SIGNAL(clicked()), this, SLOT(slotFullScreenButtonClicked()));
    m_fullScreenButton->installEventFilter(this);

    bottomWidgetLayout();
}

void CWidget::initVideoWidget()
{
    m_videoWidgetBase = new QWidget(this);
    m_videoWidgetBase->setStyleSheet("QWidget{border-top-left-radius:0px;"
                                              "border-top-right-radius:0px;"
                                              "background-color:rgb(0,0,0)}");
    m_videoWidgetBase->setMouseTracking(true);
    m_videoWidgetBase->installEventFilter(this);

    m_mainLabel = new QLabel(m_videoWidgetBase);
    m_mainLabel->setStyleSheet("background-color:rgb(0,0,0)");
    m_mainLabel->hide();
    m_mainLabel->installEventFilter(this);

    //播放窗口
    m_videoWidget = new CVideoWidget(m_videoWidgetBase);
    m_videoWidget->installEventFilter(this);

    connect(m_videoWidget, SIGNAL(signalInBottomRegion()), this, SLOT(slotInBottomRegion()));
    connect(m_videoWidget, SIGNAL(signalOutBottomRegion()), this, SLOT(slotOutBottomRegion()));
    connect(m_videoWidget, &CVideoWidget::signalInRightRegion, this, &CWidget::slotInRightRegion);
    connect(m_videoWidget, &CVideoWidget::signalOutRightRegion, this, &CWidget::slotOutRightRegion);
    connect(m_videoWidget, SIGNAL(signalMovePoint(QPoint)), this, SLOT(slotSignalMovePoint(QPoint)));
    connect(m_videoWidget, SIGNAL(signalLeftPressDown(bool)), this, SLOT(slotVideoWidgetMouseLeftDown()));

    connect(m_videoWidget, &CVideoWidget::signalDropedList, this, &CWidget::slotVideoWidgetDropedList);
    connect(m_videoWidget, &CVideoWidget::signalActionOpen, this, &CWidget::slotOpenFile);
    //connect(m_videoWidget, &VideoWidget::signalActionInfo, this, &Widget::slotVideoWidgetActionInfo);
    connect(m_videoWidget, &CVideoWidget::signalActionCapture, this, &CWidget::slotVideoWidgetActionCapture);
    connect(m_videoWidget, &CVideoWidget::signalMouseRelease, this, &CWidget::slotPlayOrPause);
    connect(m_videoWidget, SIGNAL(signalDoubleClickedOrKeyEscape(bool)),
            this, SLOT(slotVideoWidgetDoubleClickedOrKeyEscape(bool)));
    connect(m_videoWidget, SIGNAL(signalKeySpace()),
            this, SLOT(slotVideoWidgetKeySpace()));

    //播放列表的隐藏按钮
    m_hideButtonBase = new QWidget(m_videoWidget);
    m_hideButtonBase->setFixedSize(m_sizeHideButton.width(), m_sizeHideButton.height());
    m_hideButtonBase->setWindowFlags(Qt::FramelessWindowHint);
    m_hideButtonBase->setAttribute(Qt::WA_TranslucentBackground);
    m_hideButtonBase->setStyleSheet("QWidget{background-color:rgba(35, 35, 35, 0);}");
    //m_hideButtonBase->setWindowOpacity(0.7);

    m_hideButton = new QPushButton(m_hideButtonBase);
    m_hideButton->setCursor(QCursor(Qt::ArrowCursor));
    m_hideButton->setStyleSheet("QPushButton{image:url(:/images/images/right.png);"
                                           "border:none;"
                                           "padding-right:1px;"
                                           "background-color:rgba(35, 35, 35, 255);}"
                                           "QPushButton::hover{border:none;"
                                           "background-color:rgb(70,70,70);}");
    connect(m_hideButton, &QPushButton::clicked, this, &CWidget::slotHideButton);
    m_hideButton->installEventFilter(this);

    videoWidgetLayout();
}

void CWidget::initRightWidget()
{
    //窗口右侧Widget
    m_rightWidgetBase = new QWidget(this);
    m_rightWidgetBase->setGeometry(m_rectRightWidget);
    m_rightWidgetBase->setStyleSheet("QWidget{background-color: rgba(26, 26, 26, 255);}");

    m_rightWidget = new CRightWidget(m_rightWidgetBase);
    m_rightWidget->setWindowFlags(Qt::FramelessWindowHint);
    m_rightWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_rightWidget->setBackgroundColor(QColor(26, 26, 26, 255));

    //播放列表label
    m_listLabel = new QLabel(m_rightWidget);
    m_listLabel->setStyleSheet("QLabel{background-color: rgba(255, 0, 0, 0);"
                                    "color: rgba(177,177,177,255);"
                                    "font-size: 20px;}");
    m_listLabel->setText("播放列表");
    m_listLabel->installEventFilter(this);

    //顶部分割线
    m_splitLineTop = new CSplitLine(m_rightWidget);
    m_splitLineTop->setBackgroundColor(QColor(63, 63, 63, 255));
    m_splitLineTop->setLineColor(QColor(63, 63, 63, 255));
    m_splitLineTop->installEventFilter(this);

    //列表框
    m_listWidget = new CListWidget(m_rightWidget);
    m_listWidget->installEventFilter(this);

    connect(m_listWidget, SIGNAL(ItemDoubleClick(int)), this, SLOT(slotListWidgetItemDoubleClick(int)));
    connect(m_listWidget, SIGNAL(ItemDeleted(int)), this, SLOT(slotListWidgetItemDeleted(int)));
    connect(m_listWidget, &CListWidget::signalClearAll, this, &CWidget::slotListWidgetClearAll);
    connect(m_listWidget, &CListWidget::signalClearValidItem, this, &CWidget::slotListWidgetClearValidItem);
    connect(m_listWidget, &CListWidget::signalOpenDirectory, this, &CWidget::slotListWidgetOpenDirectory);
    connect(m_listWidget, SIGNAL(signalAddMedia(QStringList)), this, SLOT(slotListWidgetAddMediaFile(QStringList)));

    connect(m_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(slotCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    connect(m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotCureentItemDoubleClick(QListWidgetItem *)));
    //connect(m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, )

    //底部分割线
    m_splitLineBottom = new CSplitLine(m_rightWidget);
    m_splitLineBottom->setBackgroundColor(QColor(63, 63, 63, 255));
    m_splitLineBottom->setLineColor(QColor(63, 63, 63, 255));
    m_splitLineBottom->installEventFilter(this);

    //添加按钮
    m_addButton = new QPushButton(m_rightWidget);
    m_addButton->setToolTip("添加");
    m_addButton->setStyleSheet("QPushButton{image:url(:/images/images/add.png);"
                                           "border-radius:10px;"
                                           "background-color: transparent}"
                               "QPushButton::hover{background-color: rgb(90,90,90);"
                                                  "image:url(:/images/images/addHovered.png);}"
                               "QPushButton::menu-indicator{image:none;}"
                               );

    m_addButton->installEventFilter(this);

    //播放模式菜单
    m_menuAdd = new QMenu;
    m_menuAdd->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_menuAdd->setAttribute(Qt::WA_TranslucentBackground);

    m_actionAddFile          = m_menuAdd->addAction("添加文件");
    m_actionAddFolder        = m_menuAdd->addAction("添加文件夹");

    m_menuAdd->setStyleSheet("QMenu{border:none;"
                                    "border-radius: 5px;"
                                    "background-color:white;}"
                             "QMenu::item{border:none;"
                                          "border-radius: 10px;"
                                          "font-size:16px;"
                                          "padding-left:30px;"
                                          "background-color:white;"
                                          "height:40px;"
                                          "width:150px;}"
                             "QMenu::item:selected{background-color:rgb(140,140,240)};"
                                  );

    connect(m_actionAddFile, SIGNAL(triggered()), m_listWidget, SLOT(slotActionAddFile()));
    connect(m_actionAddFolder, SIGNAL(triggered()), m_listWidget, SLOT(slotActionAddFolder()));
    m_addButton->setMenu(m_menuAdd);

    rightWidgetLayout();
}

void CWidget::initMediaPlayer()
{
    m_mediaPlayer = new QMediaPlayer;
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_videoWidget->setMediaPlayer(m_mediaPlayer);

    m_mediaPlaylist = new QMediaPlaylist(this);
    m_mediaPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);//单个播放
    m_mediaPlayer->setPlaylist(m_mediaPlaylist);

    connect(m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(slotStateChanged(QMediaPlayer::State)));
    connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(slotPositionChanged(qint64)));
    connect(m_mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(slotDurationChanged(qint64)));
    connect(m_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotHandleError()));
    connect(m_mediaPlaylist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotPlaylistChanged(int)));

}

void CWidget::refreshLayout(QRect rectClient)
{
    //顶部widget
    m_sizeTopWidget.setWidth(rectClient.width());
    m_rectTopWidget.setRect(rectClient.left(), rectClient.top(), m_sizeTopWidget.width(), m_sizeTopWidget.height());

    //右侧widget
    m_sizeRightWidget.setHeight(rectClient.height() - m_rectTopWidget.height());
    m_rectRightWidget.setRect(rectClient.width() - m_sizeRightWidget.width(),
                             m_rectTopWidget.height(),
                             m_sizeRightWidget.width(),
                             m_sizeRightWidget.height());

    //底部widget
    int rightWidgetWidth = 0;
    if(NULL == m_rightWidgetBase)
    {
        rightWidgetWidth = m_rectRightWidget.width();
    }
    else
    {
        if(m_rightWidgetBase->isVisible())
        {
            rightWidgetWidth = m_rectRightWidget.width();
        }
        else
        {
            rightWidgetWidth = 0;
        }
    }

    m_sizeBottomWidget.setWidth(rectClient.width() - rightWidgetWidth);
    m_rectBottomWidget.setRect(rectClient.left(),
                               rectClient.height() - m_sizeBottomWidget.height(),
                               m_sizeBottomWidget.width(),
                               m_sizeBottomWidget.height());

    //中间videoWidget
    m_sizeVideoWidget.setWidth(rectClient.width() - rightWidgetWidth);
    m_sizeVideoWidget.setHeight(rectClient.height() - m_rectTopWidget.height() - m_rectBottomWidget.height());
    m_rectVideoWidget.setRect(m_rectTopWidget.left(),
                              m_rectTopWidget.height(),
                              m_sizeVideoWidget.width(),
                              m_sizeVideoWidget.height());

    m_rectHideButton.setRect(0 + m_rectVideoWidget.width() - m_sizeHideButton.width(),
                             0 + (m_rectVideoWidget.height() - m_sizeHideButton.height())/2,
                             m_sizeHideButton.width(),
                             m_sizeHideButton.height());

}

void CWidget::slotPlayMode()
{
    QAction* actCurrent = qobject_cast<QAction*>(sender());
    actCurrent->setChecked(true);

    QList<QAction*> actList = m_menuPlayMode->actions();
    int count = actList.count();
    for(int i = 0; i < count; i++)
    {
        if(actCurrent != actList.at(i))
        {
            actList.at(i)->setChecked(false);
        }
        else
        {
            m_playingMode = m_playingModeList.at(i);
        }
    }

    setPlayModel();
}

void CWidget::setPlayModel()
{
    QMediaPlaylist::PlaybackMode playingMode = QMediaPlaylist::CurrentItemOnce;

    for(int i = 0; i < m_playingModeList.count(); i++)
    {
        if(m_playingModeList.at(i) == m_playingMode)
        {
            playingMode = (QMediaPlaylist::PlaybackMode)i;
        }
    }

    switch(playingMode)
    {
        case QMediaPlaylist::CurrentItemOnce:
        {
            m_playModeButton->setStyleSheet("QPushButton{border-radius:15px;"
                                                         "image:url(:/images/images/itemOnce.png);}"
                                            "QPushButton:hover{background-color:rgb(90,90,90);"
                                                               "image:url(:/images/images/itemOnceHovered.png)}"
                                            "QPushButton::menu-indicator{image:none;}");
            m_playModeButton->setToolTip("单个播放");
            break;
        }
        case QMediaPlaylist::CurrentItemInLoop:
        {
            m_playModeButton->setStyleSheet("QPushButton{border-radius:15px;"
                                                         "image:url(:/images/images/itemLoop.png);}"
                                            "QPushButton:hover{background-color:rgb(90,90,90);"
                                                               "image:url(:/images/images/itemLoopHovered.png);}"
                                            "QPushButton::menu-indicator{image:none;}");
            m_playModeButton->setToolTip("单个循环");
            break;
        }
        case QMediaPlaylist::Sequential:
        {
            m_playModeButton->setStyleSheet("QPushButton{border-radius:15px;"
                                                         "image:url(:/images/images/sequential.png);}"
                                            "QPushButton:hover{background-color:rgb(90,90,90);"
                                                               "image:url(:/images/images/sequentialHovered.png);}"
                                            "QPushButton::menu-indicator{image:none;}");
            m_playModeButton->setToolTip("顺序播放");
            break;
        }
        case QMediaPlaylist::Loop:
        {
            m_playModeButton->setStyleSheet("QPushButton{border-radius:15px;"
                                                         "image:url(:/images/images/listLoop.png);}"
                                            "QPushButton:hover{background-color:rgb(90,90,90);"
                                                               "image:url(:/images/images/listLoopHovered.png);}"
                                            "QPushButton::menu-indicator{image:none;}");
            m_playModeButton->setToolTip("列表循环");
            break;
        }
        case QMediaPlaylist::Random:
        {
            m_playModeButton->setStyleSheet("QPushButton{border-radius:15px;"
                                                         "image:url(:/images/images/random.png);}"
                                            "QPushButton:hover{background-color:rgb(90,90,90);"
                                                               "image:url(:/images/images/randomHovered.png);}"
                                            "QPushButton::menu-indicator{image:none;}");
            m_playModeButton->setToolTip("随机播放");
            break;
        }
        default:
        {
            qDebug()<<"Error in slotPlayModeListWidget ! !";
            break;
        }    
    }
    m_mediaPlaylist->setPlaybackMode(playingMode);

}

void CWidget::slotFullScreenButtonClicked()
{
    setFullScreen(!m_videoWidget->isFullScreen());
}

void CWidget::bottomWidgetLayout()
{
    m_bottomWidgetBase->setGeometry(m_rectBottomWidget);
    m_bottomWidget->setGeometry(0, 0, m_rectBottomWidget.width(), m_rectBottomWidget.height());

    m_timeSlider->setGeometry(8, 0, m_rectBottomWidget.width() - 16, 30);
    m_stopButton->setGeometry(20, 35, 40, 40);
    m_previousVideoButton->setGeometry(70, 35, 40, 40);
    m_playButton->setGeometry(120, 30, 50, 50);
    m_nextVideoButton->setGeometry(180, 35, 40, 40);
    m_timeLabel->setGeometry(260, 40, 185, 30);
    m_volumeButton->setGeometry(460, 40, 30, 30);
    m_volumeSlider->setGeometry(495, 47, 130, 16);
    m_playModeButton->setGeometry(m_rectBottomWidget.width()-90, 45, 30, 30);
    m_fullScreenButton->setGeometry(m_rectBottomWidget.width()-45, 43, 35, 35);

    if(NULL != m_videoWidget)
    {
        QString str = "";
        if(m_videoWidget->isFullScreen())
        {
            str = "退出全屏";
            m_fullScreenButton->setStyleSheet("QPushButton{image:url(:/images/images/fullScreen_exit.png);"
                                                           "border-radius:15px;"
                                                           "background-color: transparent}"
                                              "QPushButton::hover{background-color:rgb(90,90,90);"
                                                                  "image:url(:/images/images/fullScreen_exitHovered.png);}"
                                              );
            m_fullScreenButton->setToolTip("退出全屏");
        }
        else
        {
            str = "全屏";
            m_fullScreenButton->setStyleSheet("QPushButton{image:url(:/images/images/fullScreen_alt.png);"
                                                           "border-radius:15px;"
                                                           "background-color: transparent}"
                                              "QPushButton::hover{background-color:rgb(90,90,90);"
                                                                  "image:url(:/images/images/fullScreen_altHovered.png);}"
                                              );
        }
        m_fullScreenButton->setToolTip(str);
    }
}

void CWidget::topWidgetLayout()
{
    m_topWidget->setGeometry(m_rectTopWidget);
    m_closeButton->setGeometry(m_rectTopWidget.width()-34, 10, 28, 20);     //将关闭按钮放在最右上角
    m_maximumButton->setGeometry(m_rectTopWidget.width()-68,10, 28, 20);
    m_minimumButton->setGeometry(m_rectTopWidget.width()-102, 10, 28, 20);   //按钮之间间隔6px
    m_menuButton->setGeometry(10,10,30,30);
    m_menuButton2->setGeometry(44,8,100,34);
    m_titleLabel->setGeometry(200,15,500,30);
}

void CWidget::videoWidgetLayout()
{
    m_videoWidgetBase->setGeometry(m_rectVideoWidget);
    m_mainLabel->setGeometry(0, 0, m_rectVideoWidget.width(), m_rectVideoWidget.height());
    m_videoWidget->setGeometry(0, 0, m_rectVideoWidget.width(), m_rectVideoWidget.height());

    m_hideButtonBase->setGeometry(m_rectHideButton);
    m_hideButton->setGeometry(0, 0, m_rectHideButton.width(), m_rectHideButton.height());
}

void CWidget::rightWidgetLayout()
{
    //右侧主窗口
    m_rightWidgetBase->setGeometry(m_rectRightWidget);
    m_rightWidget->setGeometry(0, 0, m_rectRightWidget.width(), m_rectRightWidget.height());

    //播放列表label
    m_sizeListLabel.setWidth(m_sizeRightWidget.width()/2);
    m_rectListLabel.setRect(20, 10, m_sizeListLabel.width(), m_sizeListLabel.height());
    m_listLabel->setGeometry(m_rectListLabel);

    //顶部分割线
    m_sizeSplitLineTop.setWidth(m_sizeRightWidget.width()-45);
    m_rectSplitLineTop.setRect(m_rectListLabel.topLeft().x()+5, m_rectListLabel.bottomLeft().y() + 2,
                            m_sizeSplitLineTop.width(),
                            m_sizeSplitLineTop.height());
    m_splitLineTop->setSizeRect(m_rectSplitLineTop);

    //列表框
    int marginBottom = 60;
    m_sizeListWidget.setHeight(m_rectRightWidget.height() - (m_rectSplitLineTop.bottomLeft().y() + 1) - marginBottom - 2);
    m_rectListWidget.setRect(0,
                             0 + m_rectSplitLineTop.bottomLeft().y() + 1,
                             m_sizeListWidget.width(),
                             m_sizeListWidget.height());
    m_listWidget->setGeometry(m_rectListWidget);

    //底部分割线
    m_sizeSplitLineBottom.setWidth(m_sizeRightWidget.width()-45);
    m_rectSplitLineBottom.setRect(m_rectSplitLineTop.topLeft().x(), 0 + m_rectRightWidget.height() - marginBottom,
                            m_sizeSplitLineBottom.width(),
                            m_sizeSplitLineBottom.height());
    m_splitLineBottom->setSizeRect(m_rectSplitLineBottom);

    //添加按钮
    int width = 20;
    int height = 20;
    QRect rect(m_rectRightWidget.width()-width-25, m_rectRightWidget.height()-height-20, width, height);
    m_addButton->setGeometry(rect);
}

void CWidget::setFullScreen(bool bIsFullScreen)
{
    m_videoWidget->setFullScreen(bIsFullScreen);

    if(!bIsFullScreen)
    {     
        setBottomWidgetFlags(false);
        setRightWidgetFlags(false);
        setState(false);
    }
    else
    {      
        int width = m_videoWidget->width();
        int height = m_videoWidget->height();

        m_rectBottomWidget.setRect(0, height - m_sizeBottomWidget.height(), width, m_sizeBottomWidget.height());
        bottomWidgetLayout();

        m_rectRightWidget.setRect(width - m_sizeRightWidget.width(), 0, m_sizeRightWidget.width(), height);
        rightWidgetLayout();

        setBottomWidgetFlags(true);
        setRightWidgetFlags(true);
        m_rightWidget->hide();
        setState(true);
    }

}

void CWidget::setState(bool bIsFullScreen)
{
    if(!bIsFullScreen)
    {
        refreshLayout(this->rect());

        rightWidgetLayout();

        videoWidgetLayout();

        topWidgetLayout();

        bottomWidgetLayout();
        m_bottomWidget->show();
        qDebug()<<"setState(!bIsFullScreen):: m_bottomWidget->show();";

        if(!m_rightWidgetBase->isHidden())
        {
            m_hideButton->setStyleSheet("QPushButton{image:url(:/images/images/right.png);"
                                                   "border:none; "
                                                   "padding-right:1px;"
                                                   "background-color:rgba(35, 35, 35, 255);}"
                                                   "QPushButton::hover{border:none;"
                                                   "background-color:rgb(70,70,70);}");
        }
        else
        {
            m_hideButton->setStyleSheet("QPushButton{image:url(:/images/images/left.png);"
                                                   "border:none;"
                                                   "padding-right:1px;"
                                                   "background-color:rgba(35, 35, 35, 255);}"
                                                   "QPushButton::hover{border:none;"
                                                   "background-color:rgb(70,70,70);}");
        }
    }
    else
    {
        int width = m_videoWidget->width();
        int height = m_videoWidget->height();

        QRect rectHideButton(0,0,0,0);
        if(!m_rightWidget->isHidden())
        {
            m_bottomWidget->hide();
            qDebug()<<"setState(bIsFullScreen):: m_bottomWidget->hide();";
            rectHideButton.setRect(0 + width - m_sizeHideButton.width() - m_sizeRightWidget.width(),
                                 0 + (height - m_sizeHideButton.height())/2,
                                 m_sizeHideButton.width(),
                                 m_sizeHideButton.height());

            m_hideButton->setStyleSheet("QPushButton{image:url(:/images/images/right.png);"
                                                   "border:none; "
                                                   "padding-right:1px;"
                                                   "background-color:rgba(35, 35, 35, 255);}"
                                                   "QPushButton::hover{border:none;"
                                                   "background-color:rgb(70,70,70);}");
        }
        else
        {
            rectHideButton.setRect(0 + width - m_sizeHideButton.width(),
                                 0 + (height - m_sizeHideButton.height())/2,
                                 m_sizeHideButton.width(),
                                 m_sizeHideButton.height());

            m_hideButton->setStyleSheet("QPushButton{image:url(:/images/images/left.png);"
                                                   "border:none;"
                                                   "padding-right:1px;"
                                                   "background-color:rgba(35, 35, 35, 255);}"
                                                   "QPushButton::hover{border:none;"
                                                   "background-color:rgb(70,70,70);}");

        }

        m_hideButtonBase->setGeometry(rectHideButton);
        m_hideButton->setGeometry(0, 0, rectHideButton.width(), rectHideButton.height());

    }

}

void CWidget::slotPlaylistChanged(int position)
{//播放列表变化，更新当前播放文件名显示
    int count = m_listWidget->count();
    for(int i = 0; i < count; i++)
    {
        QListWidgetItem* item = m_listWidget->item(i);
        if(position == i)
        {
            item->setTextColor(QColor(51,153,255));
            m_titleLabel->setText(item->text());
        }
        else
        {
            item->setTextColor(QColor(140,140,140));
        }
    }

}

QString CWidget::timeFormat(qint64 secs)
{
    int hours = secs/3600; //小时
    int mins = secs%3600/60;//分
    secs = secs%3600%60;//秒

    QString hoursFormat = hours/10 ? "%d:" : "0%d:";
    QString minFormat = mins/10 ? "%d:" : "0%d:";
    QString secsFormat = secs/10 ? "%d" : "0%d";

    QString str = QString::asprintf((hoursFormat+minFormat+secsFormat).toLatin1().data(), hours, mins, secs);
    return str;
}

void CWidget::slotVolumeButtonCliced()
{
    bool bIsMute = m_mediaPlayer->volume() ? true : false;

    isMuteVisible(bIsMute);
}

void CWidget::slotCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    /*if(NULL == previous)
    {
        current->setTextColor(QColor(51,153,255));
    }
    else
    {
        QColor color = current->textColor();
        current->setTextColor(QColor(51,153,255));
        previous->setTextColor(QColor(255,0,0));
    }*/
   // m_listWidget->indexFromItem(current).row();

    qDebug()<<"CWidget::slotCurrentItemChanged";

}

void CWidget::isMuteVisible(bool bIsMute)
{
    if(bIsMute)
    {
        m_mediaPlayer->setVolume(0);
        m_volumeButton->setToolTip("取消静音");

        m_volumeButton->setStyleSheet("QPushButton{image:url(:/images/images/none-music.png);"
                                                 "border-radius:15px; "
                                                 "background-color: transparent}"
                                    "QPushButton::hover{image:url(:/images/images/none-music-hover.png);"
                                                        "border-color:1px black; "
                                                        "background-color:rgb(90,90,90);}");
    }
    else
    {
        m_mediaPlayer->setVolume(m_volumeSlider->value());
        m_volumeButton->setToolTip("静音");

        m_volumeButton->setStyleSheet("QPushButton{image:url(:/images/images/music-voice.png);"
                                                 "border-radius:15px; "
                                                 "background-color: transparent}"
                                    "QPushButton::hover{image:url(:/images/images/music-voice-hover.png);"
                                                        "border-color:1px black; "
                                                        "background-color:rgb(90,90,90);}");
    }
}

void CWidget::setBottomWidgetFlags(bool bIsTool)
{
    if(bIsTool)
    {
        m_bottomWidget->setParent(NULL);
        m_bottomWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
        //m_bottomWidget->setAttribute(Qt::WA_TranslucentBackground);
        m_bottomWidget->setBackgroundColor(QColor(0, 0, 255, 2));

        if(!m_bottomWidget->isVisible())
        {
            m_bottomWidget->setVisible(true);
        }

        m_bottomWidget->move(m_videoWidget->mapToGlobal(QPoint(m_rectBottomWidget.left(),
                                                               m_rectBottomWidget.top())));
    }
    else
    {
        m_bottomWidget->setParent(m_bottomWidgetBase);
        m_bottomWidget->setWindowFlags(Qt::FramelessWindowHint);
        //m_bottomWidget->setAttribute(Qt::WA_TranslucentBackground);
        m_bottomWidget->setBackgroundColor(QColor(255, 255, 255, 0));

        m_bottomWidget->setGeometry(0, 0, m_rectBottomWidget.width(), m_rectBottomWidget.height());

        if(!m_bottomWidget->isVisible())
        {
            m_bottomWidget->setVisible(true);
        }
    }
    //m_bottomWidget->show();
}

void CWidget::setRightWidgetFlags(bool bIsTool)
{
    if(bIsTool)
    {
        m_rightWidget->setParent(NULL);
        m_rightWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
        //m_rightWidget->setAttribute(Qt::WA_TranslucentBackground);
        m_rightWidget->setBackgroundColor(QColor(26, 26, 26, 180));

        if(!m_rightWidget->isVisible())
        {
            m_rightWidget->setVisible(true);
        }

        m_rightWidget->move(m_videoWidget->mapToGlobal(QPoint(m_rectRightWidget.left(),
                                                               m_rectRightWidget.top())));
    }
    else
    {
        m_rightWidget->setParent(m_rightWidgetBase);
        m_rightWidget->setWindowFlags(Qt::FramelessWindowHint);
        //m_rightWidget->setAttribute(Qt::WA_TranslucentBackground);
        m_rightWidget->setBackgroundColor(QColor(26, 26, 26, 255));

        m_rightWidget->setGeometry(0, 0, m_rectRightWidget.width(), m_rectRightWidget.height());

        if(!m_rightWidget->isVisible())
        {
            m_rightWidget->setVisible(true);
        }
    }
    //m_rightWidget->show();
}

bool CWidget::isFileExist(QString fullFileName)
{
    QFileInfo fileInfo(fullFileName);
    if(fileInfo.isFile())
    {
        return true;
    }
    return false;
}

void CWidget::openTable()
{
    QSqlTableModel settingModel;
    settingModel.setTable("setting");
    settingModel.select();
    QSqlRecord record = settingModel.record(0);

    m_volumePos = record.value(0).toInt();
    m_mediaPlayer->setVolume(m_volumePos);
    m_volumeSlider->setValue(m_volumePos);
    m_playingMode = record.value(1).toString();

    QList<QAction*> actList = m_menuPlayMode->actions();
    int count = actList.count();
    for(int i = 0; i < count; i++)
    {
        if(m_playingModeList.at(i) == m_playingMode)
        {
            actList.at(i)->setChecked(true);
        }
        else
        {
            actList.at(i)->setChecked(false);
        }
    }
    setPlayModel();

    QSqlTableModel mediaTableModel;
    mediaTableModel.setTable("mediatable");
    mediaTableModel.select();
    int ret = mediaTableModel.rowCount();
    for(int i = 0; i < ret; i++)
    {
        QSqlRecord record=mediaTableModel.record(i);
        QString fileName = record.value(1).toString();
        QString filePath = record.value(2).toString();
        qint64 progress = record.value(3).toInt();

        m_mediaPlaylist->addMedia(QUrl::fromLocalFile(filePath));
        m_vectorProgress.append(progress);

        QListWidgetItem *item = new QListWidgetItem(fileName);
        item->setToolTip(fileName);
        item->setTextColor(QColor(140,140,140));

        m_listWidget->addItem(item);

        if(m_mediaPlaylist->mediaCount() < 0)
        {
            m_playButton->setEnabled(false);
        }
        else
        {
            m_playButton->setEnabled(true);
        }
    }
}

void CWidget::slotVolumeToolTipTimer()
{
    int msecsCount = m_startVolumeToolTipTime.msecsTo(QTime::currentTime());
    if(msecsCount > 3000)
    {
        m_volumeToolTip->hide();
        m_volumeToolTipTimer->stop();
    }
}

void CWidget::showVolumeToolTip(int value)
{
    if(NULL == m_volumeToolTip)
    {
        m_volumeToolTip = new CMyToolTip(m_videoWidget);
        m_volumeToolTip->setVisible(true);

        m_volumeToolTip->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
        m_volumeToolTip->setAttribute(Qt::WA_TranslucentBackground);

        m_volumeToolTip->setTextColor(QColor(255, 213, 0, 255));
        m_volumeToolTip->setBackgroundColor(QColor(38, 38, 38, 0));
        qDebug()<<"new CMyToolTip(this);";
    }
    m_volumeToolTip->move(m_videoWidget->mapToGlobal(m_volumeToolTipPos));
    if(!m_volumeToolTip->isVisible())
    {
        m_volumeToolTip->show();
    }

    if(!m_volumeToolTipTimer->isActive())
    {
        m_volumeToolTipTimer->start(50);
    }
    m_startVolumeToolTipTime = QTime::currentTime();

    m_volumeToolTip->setText(QString("音量: %1%").arg(value));
}



