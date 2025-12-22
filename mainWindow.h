#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serverdialog.h"
#include "languagedialog.h"

#include <QWidget>
#include <QSettings>
#include <QtMath>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QPainterPath>

#include <QTime>
#include <QtEndian>
#include <QUdpSocket>
#include <QHostInfo>


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void do_displayTimer_timeout();
    void do_refreshTimer_timeout();

    void on_btnMenu_clicked();

    void on_btnNormal_clicked();

    void on_btnLCD_clicked();

    void on_btnDial_clicked();

    void on_actLaunch_triggered(bool checked);

    void on_btnLocal_clicked();

    void on_btnInternet_clicked();

    void on_btnFull_clicked();

    void on_actRefresh_triggered();

    void on_actServer_triggered();

    void on_actBell_triggered(bool checked);

    void on_actLanguage_triggered();

    void on_actAbout_triggered();

private:
    Ui::MainWindow *ui;
    ServerDialog *serverDialog;
    LanguageDialog *languageDialog;
    QSettings settings;
    QTimer *displayTimer, *refreshTimer;

    QTime time; //存储时间
    QDate date; //存储日期
    bool isTimeFetched = false; //是否成功获取到时间

    QPoint mouseLastPositon;

    void fetchInternetTime();

protected:
    void paintEvent(QPaintEvent *event);

    //鼠标事件处理函数，用于拖动窗口或退出全屏
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
