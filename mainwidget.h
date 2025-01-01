#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QtMath>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>

//用于网络授时qwe
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    QSettings settings;
    QTimer *displayTimer, *refreshTimer;
    QTime time;
    QDate date;

    bool mouseIsMoving;
    QPoint mouseLastPositon;

    void fetchInternetTime();

private slots:
    void do_btnFull_clicked();
    void do_btnLocal_clicked();
    void do_btnInternet_clicked();
    void do_btnNormal_clicked();
    void do_btnLCD_clicked();
    void do_btnDial_clicked();
    void do_displayTimer_timeout();
    void do_refreshTimer_timeout();

protected:
    //鼠标事件处理函数，用于拖动窗口或退出全屏
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
};
#endif // MAINMainWidget_H
