#include "SplitLine.h"

CSplitLine::CSplitLine(QWidget *parent)// : QFrame(parent)
{
    m_frame = new QFrame(parent);
    //m_frame->setStyleSheet("color: rgba(63, 63, 63, 255);}");//63, 63, 63
    setHorizontalVertical(0);           //默认为水平分割
    m_frame->setLineWidth(2);
    m_frame->setMidLineWidth(2);

    m_strBackgroundColor = "background-color: rgba(63, 63, 63, 0);";
    m_strLineColor = "color: rgba(63, 63, 63, 255);}";
}

int CSplitLine::setHorizontalVertical(int i) //i=0时是水平分割线，i=1时是垂直分割线，默认为水平分割，已设置有默认尺寸
{
    if(0 == i)
    {
        m_frame->setFrameShape(QFrame::HLine);        // 水平分割线
        m_frame->setFrameShadow(QFrame::Plain);
        //setSizeRect(30,30,100,3);                              //设置默认尺寸
    }
    else if(1 == i)
    {
        m_frame->setFrameShape(QFrame::VLine);        // 垂直分割线
        m_frame->setFrameShadow(QFrame::Plain);
        //setSizeRect(30,30,3,100);
    }

    return i;
}

void CSplitLine::setSizeRect(int x, int y, int width, int height)
{
    m_frame->setGeometry(QRect(x, y, width, height));
}

void CSplitLine::setSizeRect(const QRect& rect)
{
    m_frame->setGeometry(rect);
}

void CSplitLine::setBackgroundColor(QColor rgba)
{
    /*QPalette palette = m_frame->palette();
    palette.setColor(QPalette::Background, rgba);
    m_frame->setPalette(palette);*/

    int red = rgba.red();
    int green = rgba.green();
    int blue = rgba.blue();
    int alpha = rgba.alpha();

    m_strBackgroundColor = QString("background-color: rgba(%1, %2, %3, %4);").
            arg(red).arg(green).arg(blue).arg(alpha);

    m_frame->setStyleSheet(getStyleSheet());

}

void CSplitLine::setLineColor(QColor rgba)
{
    int red = rgba.red();
    int green = rgba.green();
    int blue = rgba.blue();
    int alpha = rgba.alpha();

    m_strLineColor = QString("color: rgba(%1, %2, %3, %4);").
            arg(red).arg(green).arg(blue).arg(alpha);

    m_frame->setStyleSheet(getStyleSheet());
}

QString CSplitLine::getStyleSheet()
{
    QString styleSheet = "QFrame{" + m_strBackgroundColor + m_strLineColor + "}";

    return styleSheet;
}
