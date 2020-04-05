#ifndef SPLITLINE_H
#define SPLITLINE_H

#include <QObject>
#include <QFrame>

class CSplitLine : public QFrame
{
    Q_OBJECT
public:
    explicit CSplitLine(QWidget* parent = nullptr);

signals:

public slots:

public:
    int setHorizontalVertical(int i = 0);//i=0时是水平分割线，i=1时是垂直分割线，默认为水平分割
    void setSizeRect(int x, int y, int width, int height);//
    void setSizeRect(const QRect& rect);
    void setBackgroundColor(QColor rgba);
    void setLineColor(QColor rgba);
    QString getStyleSheet();

public:
    QFrame*     m_frame;
    QString     m_strBackgroundColor;
    QString     m_strLineColor;

};

#endif // SPLITLINE_H
