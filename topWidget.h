#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

class CTopWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CTopWidget(QWidget *parent = nullptr);

private:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *event);

signals:
    void signalMovePoint(QPoint);
    void signalLeftPressDown();
    void signalDoubleClicked();

public slots:

private:
    bool isLeftPressDown;  // 判断左键是否按下
    QPoint dragPosition;    // 窗口移动拖动时需要记住的点
};

#endif // TOPWIDGET_H
