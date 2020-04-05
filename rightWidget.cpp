#include "rightWidget.h"
#include <QPainter>
#include <QStyleOption>

CRightWidget::CRightWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    m_strBackgroundColor = "QWidget{background-color: rgba(255, 255, 255, 0);}";
}

void CRightWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
}

void CRightWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
}

void CRightWidget::paintEvent(QPaintEvent *event)
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

void CRightWidget::showEvent(QShowEvent *event)
{
    //this->setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(event);
}

void CRightWidget::show()
{
    QWidget::show();
}

void CRightWidget::hide()
{
    QWidget::hide();
}

void CRightWidget::move(QPoint curPos)
{
    QWidget::move(curPos);
}

void CRightWidget::setBackgroundColor(QColor rgb)
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

QString CRightWidget::getStyleSheet()
{
    QString styleSheet = "QWidget{" + m_strBackgroundColor + "}";

    return styleSheet;
}
