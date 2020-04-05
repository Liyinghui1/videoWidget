#include "MyToolTip.h"
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

CMyToolTip::CMyToolTip(QWidget *parent) : QWidget(parent)
{
    //m_ptrParent = parent;
    //m_rectParent = parent->rect();
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(/* Qt::WA_DeleteOnClose| */Qt::WA_TranslucentBackground);

    //this->setFixedHeight(100);
    //this->setFixedWidth(400);

    m_label = new QLabel(this);
    m_label->setAutoFillBackground(true);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setGeometry(0, 0, this->width(), this->height());

//    QHBoxLayout* mLayout = new QHBoxLayout(this);
//    mLayout->addWidget(m_label);

//    m_label->setStyleSheet("QLabel{"
//                           "background-color: rgba(38, 38, 38, 0);"
//                           "color: rgba(218, 218, 218, 255);"
//                           "font-size: 25px;
//                           "font-family: Comic Sans MS;}"
//                           );

    this->installEventFilter(this);
    this->setStyleSheet("QWidget{border-radius:5px;"
                        "background-color: rgba(38, 38, 38, 255);}");//transparent

    m_strBackgroundColor = "background-color: rgba(38, 38, 38, 0);";
    m_strTextColor = "color: rgba(218, 218, 218, 255);";
    m_strFontSize = "font-size: 20px;";
    m_strFontFamily = "font-family: Comic Sans MS;";

}

void CMyToolTip::setText(const QString &text)
{
    m_label->setText(text);
}

bool CMyToolTip::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == this)
    {
//        if (QEvent::WindowDeactivate == event->type())
//        {
//            this->close();
//            event->accept();
//            return true;
//        }
    }
    return QWidget::eventFilter(obj, event);
}

void CMyToolTip::show()
{
    QWidget::show();
}

void CMyToolTip::hide()
{
    QWidget::hide();
}

void CMyToolTip::move(QPoint curPos)
{
    QWidget::move(curPos);
}

void CMyToolTip::setTextColor(QColor rgb)
{
    /*QPalette palette = m_label->palette();
    palette.setColor(QPalette::WindowText, rgb);
    m_label->setPalette(palette);*/

    int red = rgb.red();
    int green = rgb.green();
    int blue = rgb.blue();
    int alpha = rgb.alpha();

    m_strTextColor = QString("color: rgba(%1, %2, %3, %4);").
            arg(red).arg(green).arg(blue).arg(alpha);

    m_label->setStyleSheet(getStyleSheet());
}

void CMyToolTip::setBackgroundColor(QColor rgb)
{
    /*QPalette palette = m_label->palette();
    palette.setColor(QPalette::Background,  rgb);
    m_label->setPalette(palette);*/

    int red = rgb.red();
    int green = rgb.green();
    int blue = rgb.blue();
    int alpha = rgb.alpha();

    m_strBackgroundColor = QString("background-color: rgba(%1, %2, %3, %4);").
            arg(red).arg(green).arg(blue).arg(alpha);

    m_label->setStyleSheet(getStyleSheet());
}

void CMyToolTip::setFont(QFont font)
{
    //m_label->setFont(font);

    m_strFontSize = QString("font-size: %1px;").arg(font.pointSize());
    m_strFontFamily = "font-family: " + font.family();

    m_label->setStyleSheet(getStyleSheet());
}

QString CMyToolTip::getStyleSheet()
{
    QString styleSheet = "QLabel{"+m_strBackgroundColor + m_strTextColor + m_strFontSize + m_strFontFamily + "}";

    return styleSheet;
}

