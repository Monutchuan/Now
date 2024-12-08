#include "tlcdcurrenttime.h"
#include "ui_tlcdcurrenttime.h"
#include <QTimer>

TLCDCurrentTime::TLCDCurrentTime(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TLCDCurrentTime)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(1);
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(do_timer_timeout()));
}

TLCDCurrentTime::~TLCDCurrentTime()
{
    delete ui;
}

QColor TLCDCurrentTime::color()
{
    return vColor;
}

void TLCDCurrentTime::setColor(QColor fColor)
{
    QPalette palette;
    palette.setColor(QPalette::WindowText,fColor);
    vColor = fColor;
    ui->lcdHour->setPalette(palette);
    ui->lcdMinute->setPalette(palette);
    ui->lcdSecond->setPalette(palette);
}

void TLCDCurrentTime::do_timer_timeout()
{
    if(time != QTime::currentTime() && timer->interval() == 1)
        timer->setInterval(1000);
    time = QTime::currentTime();
    ui->lcdHour->display(time.hour());
    ui->lcdMinute->display(time.minute());
    ui->lcdSecond->display(time.second());
}
