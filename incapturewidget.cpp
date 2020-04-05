#include "InCaptureWidget.h"

#include<QDebug>
#include<QWidget>

CInCaptureWidget::CInCaptureWidget(QWidget *parent) : QWidget(parent)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    //this->setStyleSheet("QWidget{background-color:rgb(228,228,228); }");
}


void CInCaptureWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(isLeftPressDown)  //
    {
         emit signalMovePoint(event->globalPos()-dragPosition);
    }
}

void CInCaptureWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isLeftPressDown = false;
        this->releaseMouse();
    }
}

void CInCaptureWidget::mousePressEvent(QMouseEvent *event)
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

