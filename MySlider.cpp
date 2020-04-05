#include "MySlider.h"
#include<QWidget>
#include<QSlider>
#include<QDebug>
#include<QCursor>
#include <QString>
#include <QToolTip>
#include <QCursor>

CMySlider::CMySlider(Qt::Orientation orientation, QWidget* parent):QSlider(orientation, parent)
{
    this->setMouseTracking(true);

    this->setCursor(QCursor(Qt::PointingHandCursor));

    /*this->setStyleSheet("QSlider{ background-color:transparent}"
                        "QSlider::add-page:horizontal{ background-color: rgb(87, 97, 106);  height:4px; }"
                        "QSlider::sub-page:horizontal{ background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(231,80,229, 255), stop:1 rgba(7,208,255, 255));"
                                                       "height: 4px;}"

                        "QSlider::add-page:hover:horizontal{ background-color: rgb(87, 97, 106);"
                                                             "height:6px; }"
                        "QSlider::sub-page:hover:horizontal{ background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(231,80,229, 255), stop:1 rgba(7,208,255, 255));"
                                                             "height:6px;}"

                        "QSlider::handle:horizontal{ margin-top:-2px; "
                                                    "margin-bottom:-2px;"
                                                    "border-radius:4px;  "
                                                    "background: rgb(222,222,222); "
                                                    "width:14px;"
                                                    "height:18px;}"

                        "QSlider::groove:horizontal{ background:transparent;"
                                                     "height:4px}"
                        "QSlider::groove:hover:horizontal{ background:transparent;"
                                                           "height:6px}"
                        );*/
    this->setStyleSheet("QSlider{ background-color:rgba(125, 0, 0, 0);}"
                        "QSlider::add-page:Horizontal{ background-color: rgba(76, 76, 76, 255);"
                                                       "height:5px;}"
                         "QSlider::sub-page:Horizontal{ background-color: rgb(51, 153, 255);"
                                                        "height:5px;}"
                         "QSlider::groove:Horizontal{ background-color: rgba(0, 0, 0, 0);"
                                                      "height:6px;}"
                         "QSlider::handle:Horizontal{ height:16px;"
                                                      "width:16px;"
                                                      "border-image: url(:/images/images/circle1.png);"
                                                      "margin: -5 -1px;}"
                        );

   /* "QToolTip{border: none;"
              "background-color: rgba(38, 38, 38, 255);"
              "color: rgba(0, 0, 255, 255);"
              "font-size:30px;}" */

    m_bIsLeftPressDown = false;
    m_bIsSliderPressed = false;
    m_bIsDragSlider = false;
    m_LeftPressPointX = 0;
    m_mouseMovePointX = 0;

    connect(this, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));

    this->installEventFilter(this);

    m_toolTip = NULL;

    m_bIsToolTipVisible = false;

}

void CMySlider::mouseMoveEvent(QMouseEvent *event)
{  
    qint64 mousePosX = event->pos().x();
    qint64 duration = this->maximum();
    qint64 width = this->size().width();

    //使其保持在一定范围内
    if(mousePosX < 0)
    {
        mousePosX = 0;
    }

    if(mousePosX > width)
    {
        mousePosX = width;
    }

    qint64 position = duration * mousePosX / width;

    if(m_bIsLeftPressDown && m_bIsSliderPressed)
    {
        m_bIsDragSlider = true;

        emit signalPressPosition(position);
    }
    else
    {
        m_mouseMovePointX = event->pos().x();
        m_bIsDragSlider = false;
    }

    if(m_bIsToolTipVisible)
    {
        emit signalMouseMovePosition(position);

        QPoint curPos(this->rect().left(), this->rect().top() - m_toolTip->height());
        if(mousePosX <=  this->rect().left() + m_toolTip->width()/2)
        {
            curPos.setX(this->rect().left());
        }
        else if(mousePosX >=  this->rect().left() + this->width() - m_toolTip->width()/2)
        {
            curPos.setX(this->rect().left() + this->width() - m_toolTip->width());
        }
        else
        {
            curPos.setX(mousePosX - m_toolTip->width()/2);
        }

        m_toolTip->move(mapToGlobal(curPos));
    }
}

void CMySlider::mousePressEvent(QMouseEvent *event)
{
    m_bIsLeftPressDown = true;
    qint64 mousePosX = event->pos().x();
    qint64 duration = this->maximum();
    qint64 width = this->size().width();

    m_LeftPressPointX = mousePosX;

    //使其保持在一定范围内
    if(mousePosX < 0)
    {
        mousePosX = 0;
    }

    if(mousePosX > width)
    {
        mousePosX = width;
    }

    m_pressedPosition = duration * mousePosX / width;

    QSlider::mousePressEvent(event);

}

void CMySlider::mouseReleaseEvent(QMouseEvent *event)
{
    m_bIsLeftPressDown = false;

    if(!m_bIsDragSlider && m_LeftPressPointX == m_mouseMovePointX)
    {
        emit signalPressPosition(m_pressedPosition);
    }

    QSlider::mouseReleaseEvent(event);
}

void CMySlider::slotSliderPressed()
{
    m_bIsSliderPressed = true;
    //qDebug()<<"CMySlider::slotSliderPressed()";
}

void CMySlider::slotSliderReleased()
{
    m_bIsSliderPressed = false;
    //qDebug()<<"CMySlider::slotSliderReleased()";
}

bool CMySlider::event(QEvent *event)
{
    return QSlider::event(event);
}

void CMySlider::setToolTipText(const QString toolTip)
{
    m_bIsToolTipVisible = true;
    m_toolTip->setText(toolTip);
}

CMyToolTip* CMySlider::getToolTip()
{
    return m_toolTip;
}

void CMySlider::setToolTipVisible(bool bIsToolTipVisible)
{
    m_bIsToolTipVisible = bIsToolTipVisible;
}

bool CMySlider::getToolTipVisible()
{
    return m_bIsToolTipVisible;
}

bool CMySlider::eventFilter(QObject *target, QEvent *event)
{
    if (target == this)
    {
        if (QEvent::Enter == event->type())
        {
            if(m_bIsToolTipVisible)
            {
                if(NULL == m_toolTip)
                {
                    m_toolTip = new CMyToolTip(this);
                    m_toolTip->setTextColor(QColor(218, 218, 218, 255));
                    m_toolTip->setBackgroundColor(QColor(38, 38, 38, 2));
                    QFont font;
                    font.setFamily("华文行楷");
                    font.setPointSize(30);
                    m_toolTip->setFont(font);
                }
                m_toolTip->show();
            }
        }

        if(QEvent::Leave == event->type())
        {
            if(m_bIsToolTipVisible)
            {
                m_toolTip->close();
                //m_toolTip = NULL;
                //return true;
            }
        }
    }
    return QSlider::eventFilter(target, event);
}

void CMySlider::leaveEvent(QEvent * event)
{
    QSlider::leaveEvent(event);
}

void CMySlider::enterEvent(QEvent *event)
{
    QSlider::enterEvent(event);
}
