#ifndef MYTOOLTIP_H
#define MYTOOLTIP_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class CMyToolTip : public QWidget
{
   Q_OBJECT
public:
    explicit CMyToolTip(QWidget *parent = nullptr);

signals:

public slots:

public:
    void setText(const QString &text);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

public:
    void show();
    void hide();
    void move(QPoint curPos);
    void setTextColor(QColor rgb);
    void setBackgroundColor(QColor rgb);
    void setFont(QFont font);
    QString getStyleSheet();

private:
    QLabel*     m_label;
    QRect       m_rectParent;
    QWidget*    m_ptrParent;

    QPushButton*     m_button1;

    QString     m_strTextColor;
    QString     m_strBackgroundColor;
    QString     m_strFontSize;
    QString     m_strFontFamily;

};

#endif // MYTOOLTIP_H
