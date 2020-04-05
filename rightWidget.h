#ifndef RIGHTWIDGET_H
#define RIGHTWIDGET_H

#include <QWidget>

class CRightWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRightWidget(QWidget *parent = nullptr);

protected:
    void    enterEvent(QEvent *);
    void    leaveEvent(QEvent *);
    void    paintEvent(QPaintEvent *event);
    void    showEvent(QShowEvent *event);

signals:

public slots:

public:
    void    show();
    void    hide();
    void    move(QPoint curPos);
    void    setBackgroundColor(QColor rgb);
    QString getStyleSheet();

private:
    QString     m_strBackgroundColor;

};

#endif // RIGHTWIDGET_H
