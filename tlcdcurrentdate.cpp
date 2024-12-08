#include "tlcdcurrentdate.h"
#include "ui_tlcdcurrentdate.h"
#include <QTimer>

TLCDCurrentDate::TLCDCurrentDate(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TLCDCurrentDate)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(1);
    connect(timer, SIGNAL(timeout()), this, SLOT(do_timer_timeout()));
}

TLCDCurrentDate::~TLCDCurrentDate()
{
    delete ui;
}

QColor TLCDCurrentDate::color()
{
    return vColor;
}

void TLCDCurrentDate::setColor(QColor fColor)
{
    QPalette palette;
    palette.setColor(QPalette::WindowText,fColor);
    vColor = fColor;
    ui->lcdYear->setPalette(palette);
    ui->lcdMonth->setPalette(palette);
    ui->lcdDay->setPalette(palette);
}

void TLCDCurrentDate::do_timer_timeout()
{
    if(time != QTime::currentTime() && timer->interval() == 1)
        timer->setInterval(1000);
    date = QDate::currentDate();
    ui->lcdYear->display(date.year());
    ui->lcdMonth->display(date.month());
    ui->lcdDay->display(date.day());
}
