#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include "tlcdcurrenttime.h"
#include "tlcdcurrentdate.h"

//用于网络授时
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QTimer *displayTimer, *refreshTimer;
    TLCDCurrentTime *lcdTime;
    TLCDCurrentDate *lcdDate;
    QTime time;
    QDate date;

    bool mouseIsMoving;
    QPoint mouseLastPositon;

    void fetchInternetTime();

private slots:
    void do_btnFull_clicked();
    void do_btnLocal_clicked();
    void do_btnInternet_clicked();
    void do_displayTimer_timeout();
    void do_refreshTimer_timeout();

protected:
    //鼠标事件处理函数，用于拖动窗口或退出全屏
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
};
#endif // WIDGET_H
