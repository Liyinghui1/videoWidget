#include "BottomWidget.h"
#include <QDebug>
#include <QEvent>
#include <QPalette>
#include <QPainter>
#include <QStyleOption>
#include <QHBoxLayout>

CBottomWidget::CBottomWidget(QWidget *parent) : QWidget(parent)
{
    //this->setMouseTracking(true);
    //QPalette pal = palette();
    //pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    //setPalette(pal);

    //setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    //QHBoxLayout* mLayout = new QHBoxLayout(this);

    //this->setStyleSheet("QWidget{background-color: rgba(0, 0, 255, 255);}");

    m_strBackgroundColor = "QWidget{background-color: rgba(255, 255, 255, 0);}";
}

void CBottomWidget::enterEvent(QEvent *event)
{
    qDebug()<<"CBottomWidget::enter";
    QWidget::enterEvent(event);
}

void CBottomWidget::leaveEvent(QEvent *event)
{
    qDebug()<<"CBottomWidget::leaveEvent";
    QWidget::leaveEvent(event);
}

void CBottomWidget::paintEvent(QPaintEvent *event)
{
    //创建绘画对象
    QPainter painter(this);
    //为窗口添加一个半透明的矩形遮罩
    //painter.fillRect(this->rect(),QColor(0,0,255,20));

    QStyleOption opt;
    opt.init(this);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QWidget::paintEvent(event);
}

void CBottomWidget::showEvent(QShowEvent *event)
{
    //this->setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(event);
}

void CBottomWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint topLeftPoint (0, this->height()-140);
    QPoint rightBottomPoint(this->width(), this->height());
    topLeftPoint  = this->mapToGlobal(topLeftPoint);
    rightBottomPoint = this->mapToGlobal(rightBottomPoint);
    QPoint mouseGlobalPoint = this->mapToGlobal(event->pos());

    if(m_bIsLeftPressDown)  //
    {
         emit signalMovePoint(event->globalPos() - m_dragPosition);
    }
    //QWidget::mouseMoveEvent(event);
}

void CBottomWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bIsLeftPressDown = false;
        this->releaseMouse();
    }

    //QWidget::mouseReleaseEvent(event);
}

void CBottomWidget::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::LeftButton:
    {
        m_bIsLeftPressDown = true;
        emit signalLeftPressDown();
        m_dragPosition = event->globalPos();
        break;
    }
    case Qt::RightButton:
        break;
    default:
        break;
    }
    //QWidget::mousePressEvent(event);
}

void CBottomWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void CBottomWidget::show()
{
    QWidget::show();
}

void CBottomWidget::hide()
{
    QWidget::hide();
}

void CBottomWidget::move(QPoint curPos)
{
    QWidget::move(curPos);
}

void CBottomWidget::setBackgroundColor(QColor rgb)
{
    /*QPalette palette = this->palette();
    palette.setColor(QPalette::Background,  rgb);
    this->setPalette(palette);*/

    int red = rgb.red();
    int green = rgb.green();
    int blue = rgb.blue();
    int alpha = rgb.alpha();

    m_strBackgroundColor = QString("background-color: rgba(%1, %2, %3, %4);").
            arg(red).arg(green).arg(blue).arg(alpha);

    this->setStyleSheet(getStyleSheet());
}

QString CBottomWidget::getStyleSheet()
{
    QString styleSheet = "QWidget{" + m_strBackgroundColor + "}";

    return styleSheet;
}


