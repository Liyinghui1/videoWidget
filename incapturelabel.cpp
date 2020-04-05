#include "InCaptureLabel.h"


#include<QDebug>

CInCaptureLabel::CInCaptureLabel(QWidget *parent) : QLabel(parent)
{

    this->setCursor(QCursor(Qt::ArrowCursor));
}


void CInCaptureLabel::mouseMoveEvent(QMouseEvent *event)
{
    if(isLeftPressDown)  //
    {
         emit signalMovePoint(event->globalPos()-dragPosition);
    }
}

void CInCaptureLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isLeftPressDown = false;
        this->releaseMouse();

    }
}

void CInCaptureLabel::mousePressEvent(QMouseEvent *event)
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

