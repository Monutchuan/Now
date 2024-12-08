#ifndef TLCDCURRENTDATE_H
#define TLCDCURRENTDATE_H

#include <QWidget>
#include <QTime>

namespace Ui {
class TLCDCurrentDate;
}

class TLCDCurrentDate : public QWidget
{
    Q_OBJECT

public:
    explicit TLCDCurrentDate(QWidget *parent = nullptr);
    ~TLCDCurrentDate();
    QColor color();
    void setColor(QColor fColor);

private:
    Ui::TLCDCurrentDate *ui;
    QTimer *timer;
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QColor vColor = Qt::black;
private slots:
    void do_timer_timeout();
};

#endif // TLCDCURRENTDATE_H
