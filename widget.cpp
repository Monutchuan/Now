#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(600,350);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    connect(ui->btnFull, SIGNAL(clicked()), this, SLOT(do_btnFull_clicked()));
    connect(ui->btnLocal, SIGNAL(clicked()), this, SLOT(do_btnLocal_clicked()));
    connect(ui->btnInternet, SIGNAL(clicked()), this, SLOT(do_btnInternet_clicked()));

    //lcdTime = new TLCDCurrentTime(ui->widget);
    displayTimer = new QTimer(this); //用于显示时间
    displayTimer->setTimerType(Qt::PreciseTimer);
    displayTimer->setInterval(1); //一毫秒（临时，用于校准）
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(do_displayTimer_timeout()));
    displayTimer->start();

    refreshTimer = new QTimer(this); //用于更新时间
    refreshTimer->setInterval(180000); //三十分钟
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(do_refreshTimer_timeout()));
    refreshTimer->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::fetchInternetTime()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply* reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QString data = reply->readAll();
            // 假设获取到的时间数据格式为 "yyyy-MM-ddTHH:mm:ssZ"
            QDateTime dateTime = QDateTime::fromString(data, Qt::ISODate);
            qDebug()<<data;
            time = dateTime.time();
            date = dateTime.date();
        }
        else
            qDebug()<<"Failed to get network time.";
        reply->deleteLater();
    });
    QNetworkRequest request(QUrl("http://worldclockapi.com/api/json/utc/now"));
    manager->get(request);
}

void Widget::do_btnFull_clicked()
{//按下“全屏”按钮
    if(isFullScreen())
    {
        showNormal();

        ui->widget_2->setHidden(false);
        ui->widget_4->setHidden(false);

        QFont font;
        font = ui->labTime->font();
        font.setPointSize(font.pointSize()/2);
        ui->labTime->setFont(font);

        font = ui->labDate->font();
        font.setPointSize(font.pointSize()/2);
        ui->labDate->setFont(font);

        ui->widget_3->setStyleSheet("QWidget#widget_3{background-color: rgba(0,0,0,100);"
                                    "border-radius: 10px;}");
        update();
    }
    else
    {
        showFullScreen();

        ui->widget_2->setHidden(true);
        ui->widget_4->setHidden(true);

        QFont font;
        font = ui->labTime->font();
        font.setPointSize(font.pointSize()*2);
        ui->labTime->setFont(font);

        font = ui->labDate->font();
        font.setPointSize(font.pointSize()*2);
        ui->labDate->setFont(font);

        ui->widget_3->setStyleSheet("QWidget#widget_3{background-color: rgba(0,0,0,100);}");
        update();
    }
}

void Widget::do_btnLocal_clicked()
{//按下“本地”按钮
    ui->btnLocal->setEnabled(false);
    ui->btnInternet->setEnabled(true);
    displayTimer->setInterval(1);
}

void Widget::do_btnInternet_clicked()
{
    ui->btnLocal->setEnabled(true);
    ui->btnInternet->setEnabled(false);
    fetchInternetTime();
}

void Widget::do_displayTimer_timeout()
{
    if(time != QTime::currentTime() && displayTimer->interval() == 1)
        displayTimer->setInterval(1000);
    time = QTime::currentTime();
    date = QDate::currentDate();
    ui->labTime->setText(time.toString("hh:mm:ss"));
    ui->labDate->setText(date.toString("yyyy.MM.dd"));
}

void Widget::do_refreshTimer_timeout()
{
    //刷新计时器
    displayTimer->setInterval(1);

    if(ui->btnLocal->isEnabled())
    {
        fetchInternetTime();
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{//鼠标按键被按下
    if(isFullScreen()) do_btnFull_clicked();
    else
    {
        if (event->button() == Qt::LeftButton)
        {
            mouseIsMoving = true;
            mouseLastPositon = event->globalPosition().toPoint() - this->pos();
        }
        return QWidget::mousePressEvent(event);
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{//鼠标按下左键移动
    if(!isFullScreen())
    {
        QPoint eventPos=event->globalPosition().toPoint();

        if (mouseIsMoving && (event->buttons() & Qt::LeftButton)
            && (eventPos-mouseLastPositon).manhattanLength() > QApplication::startDragDistance())
        {
            move(eventPos-mouseLastPositon);
            mouseLastPositon = eventPos - this->pos();
        }
        return QWidget::mouseMoveEvent(event);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{ //鼠标按键被释放
    if(!isFullScreen())
    {
        mouseIsMoving=false;     //停止移动
        event->accept();
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    if(isFullScreen()) do_btnFull_clicked();
}
