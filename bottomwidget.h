#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <QWidget>
#include<QEvent>
#include<QMouseEvent>

//主窗口底部的控件，用于安放上一曲下一曲等按钮
class CBottomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CBottomWidget(QWidget *parent = 0);

protected:
    void    mouseDoubleClickEvent(QMouseEvent *);
    void    mousePressEvent(QMouseEvent *);
    void    mouseMoveEvent(QMouseEvent *);
    void    mouseReleaseEvent(QMouseEvent *);
    void    enterEvent(QEvent *);
    void    leaveEvent(QEvent *);
    void    paintEvent(QPaintEvent *event);
    void    showEvent(QShowEvent *event);

signals:
    void    signalMovePoint(QPoint);
    void    signalLeftPressDown();
    void    signalDoubleClicked();

public slots:



public:
    void    show();
    void    hide();
    void    move(QPoint curPos);
    void    setBackgroundColor(QColor rgb);
    QString getStyleSheet();

private:
    bool    m_bIsLeftPressDown;  //判断左键是否按下
    QPoint  m_dragPosition;      //窗口移动拖动时需要记住的点

    QString     m_strBackgroundColor;
};

#endif // GROUPBOX_H
