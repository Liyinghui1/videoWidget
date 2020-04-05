#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QWidget>
#include<QListWidget>
#include<QMouseEvent>
#include<QEvent>
#include<QScrollBar>
#include<QPoint>
#include<QString>
#include<QContextMenuEvent>

//右侧播放列表控件，显示打开过的文件
class CListWidget : public QListWidget
{
    Q_OBJECT
public:
    CListWidget(QWidget * parent = 0) ;

protected:
    void region(const QPoint &cursorGlobalPoint);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *menuEvent);
    void paintEvent(QPaintEvent *event);

signals:
    void ItemDoubleClick(int index);
    void ItemDeleted(int index);
    void signalClearAll();
    void signalActionOpen();
    void signalClearValidItem();
    void signalOpenDirectory(int n);
    void signalAddMedia(QStringList strList);

public slots:
    void slotActionDeleteItem();
    void slotActionClearValidItem();
    void slotActionClearAll();
    void slotActionPlay();
    void slotActionOpenDirectory();
    void slotActionAddFile();
    void slotActionAddFolder();
    void slotCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void slotItemPressed(QListWidgetItem *current);

private:
    int     m_currentIndex;
    QColor  m_curTextColor;
};

#endif // LISTWIDGET_H
