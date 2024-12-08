#ifndef TLCDCURRENTTIME_H
#define TLCDCURRENTTIME_H

#include <QWidget>
#include <QTime>

namespace Ui {
class TLCDCurrentTime;
}

class TLCDCurrentTime : public QWidget
{
    Q_OBJECT

public:
    explicit TLCDCurrentTime(QWidget *parent = nullptr);
    ~TLCDCurrentTime();
    QColor color();
    void setColor(QColor fColor);

private:
    Ui::TLCDCurrentTime *ui;
    QTimer *timer;
    QTime time = QTime::currentTime();
    QColor vColor = Qt::black;

private slots:
    void do_timer_timeout();
};

#endif // TLCDCURRENTTIME_H
