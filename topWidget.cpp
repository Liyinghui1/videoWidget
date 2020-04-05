#include "topWidget.h"
#include <QDebug>
#include <QEvent>
#include <QPalette>
#include <QPainter>
#include <QStyleOption>

CTopWidget::CTopWidget(QWidget *parent) : QWidget(parent)
{
    //this->setMouseTracking(true);
    //setAttribute(Qt::WA_TranslucentBackground, true);
//    QPalette pal = palette();
//    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
//    setPalette(pal);
    //setWindowOpacity(0.7);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //this->setAttribute(Qt::WA_TranslucentBackground);
    //this->setAutoFillBackground(true);
    //this->setStyleSheet("QWidget{background-color:rgb(33,33,33)}");
}

void CTopWidget::enterEvent(QEvent *event)
{
    //qDebug()<<"enter";
}
void CTopWidget::leaveEvent(QEvent *event)
{
    //qDebug()<<"leave";
}

void CTopWidget::paintEvent(QPaintEvent *event)
{
    //创建绘画对象
    //QPainter painter(this);
    //为窗口添加一个半透明的矩形遮罩
    //painter.fillRect(this->rect(),QColor(0,0,0,0));

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void CTopWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint topLeftPoint (0, this->height()-140);
    QPoint rightBottomPoint(this->width(), this->height());
    topLeftPoint  = this->mapToGlobal(topLeftPoint);
    rightBottomPoint = this->mapToGlobal(rightBottomPoint);
    QPoint mouseGlobalPoint = this->mapToGlobal(event->pos());

    static int cnt = 0;
    qDebug()<<"TopWidget::mouseMoveEvent: "<<cnt++;

    if(isLeftPressDown)  //
    {
         emit signalMovePoint(event->globalPos()-dragPosition);
    }
}

void CTopWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isLeftPressDown = false;
        this->releaseMouse();
    }
}

void CTopWidget::mousePressEvent(QMouseEvent *event)
{

    switch(event->button()) {
    case Qt::LeftButton:
    {
        isLeftPressDown = true;
        emit signalLeftPressDown();
        dragPosition = event->globalPos();
        break;
    }
    case Qt::RightButton:
        break;
    default:
        break;
    }
}

void CTopWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    //setFullScreen(!isFullScreen());
    //event->accept();
    if(event->button() == Qt::LeftButton)
    {
        qDebug()<<"emit signalDoubleClicked";
        emit signalDoubleClicked();
    }

}
