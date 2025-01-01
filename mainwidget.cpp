#include "MainWidget.h"
#include "ui_MainWidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setFixedSize(800,500);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    ui->HLayTime->setHidden(true);
    connect(ui->btnFull, SIGNAL(clicked()), this, SLOT(do_btnFull_clicked()));
    connect(ui->btnLocal, SIGNAL(clicked()), this, SLOT(do_btnLocal_clicked()));
    connect(ui->btnInternet, SIGNAL(clicked()), this, SLOT(do_btnInternet_clicked()));
    connect(ui->btnNormal, SIGNAL(clicked()), this, SLOT(do_btnNormal_clicked()));
    connect(ui->btnLCD, SIGNAL(clicked()), this, SLOT(do_btnLCD_clicked()));
    connect(ui->btnDial, SIGNAL(clicked()), this, SLOT(do_btnDial_clicked()));

    //ui->lcdTime = new TLCDCurrentTime(ui->MainWidget);
    displayTimer = new QTimer(this); //用于显示时间
    displayTimer->setTimerType(Qt::PreciseTimer);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(do_displayTimer_timeout()));
    displayTimer->start(1000);

    refreshTimer = new QTimer(this); //用于更新时间
    refreshTimer->setInterval(180000); //三十分钟
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(do_refreshTimer_timeout()));
    refreshTimer->start();

    int style = settings.value("DateTimeStyle", 0).toInt();
    switch(style)
    {
    case 0:
        ui->btnNormal->setChecked(true);
        do_btnNormal_clicked();
        break;
    case 1:
        ui->btnLCD->setChecked(true);
        do_btnLCD_clicked();
        break;
    case 2:
        ui->btnDial->setChecked(true);
        do_btnDial_clicked();
    }

    //QMenu *menu = new QMenu(this);
    //menu->addAction(ui->actLaunch);
    //ui->btnMenu->setMenu(menu);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::fetchInternetTime()
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
    QNetworkRequest request(QUrl("http://worldtimeapi.org/api/timezone/Etc/UTC"));
    manager->get(request);
}

void MainWidget::do_btnFull_clicked()
{//按下“全屏”按钮
    if(isFullScreen())
    {
        showNormal();

        ui->HLayTitle->setHidden(false);
        ui->HLayStyle->setHidden(false);
        ui->HLayTime->setHidden(true);

        QFont font;
        font = ui->labTime->font();
        font.setPointSize(font.pointSize()/2);
        ui->labTime->setFont(font);

        font = ui->labDate->font();
        font.setPointSize(font.pointSize()/2);
        ui->labDate->setFont(font);

        ui->lcdTime->setMinimumHeight(160);
        ui->lcdDate->setMinimumHeight(110);

        ui->VLayMain->setStyleSheet("QWidget#VLayMain{background-color: rgba(0,0,0,0.5);"
                                    "border-radius: 20px;}");
        update();
    }
    else
    {
        showFullScreen();

        ui->HLayTitle->setHidden(true);
        ui->HLayStyle->setHidden(true);

        QFont font;
        font = ui->labTime->font();
        font.setPointSize(font.pointSize()*2);
        ui->labTime->setFont(font);

        font = ui->labDate->font();
        font.setPointSize(font.pointSize()*2);
        ui->labDate->setFont(font);

        ui->lcdTime->setMinimumHeight(320);
        ui->lcdDate->setMinimumHeight(220);

        ui->VLayMain->setStyleSheet("QWidget#VLayMain{background-color: rgba(0,0,0,0.5);}");
        update();
    }
}

void MainWidget::do_btnLocal_clicked()
{//按下“本地”按钮
    ui->btnLocal->setEnabled(false);
    ui->btnInternet->setEnabled(true);
    displayTimer->setInterval(1);
}

void MainWidget::do_btnInternet_clicked()
{
    ui->btnLocal->setEnabled(true);
    ui->btnInternet->setEnabled(false);
    fetchInternetTime();
}

void MainWidget::do_btnNormal_clicked()
{
    //ui->btnNormal->clicked(true);
    do_displayTimer_timeout();
    ui->lcdTime->setHidden(true);
    ui->lcdDate->setHidden(true);
    ui->labTime->setHidden(false);
    ui->labDate->setHidden(false);
    settings.setValue("DateTimeStyle", 0);
    repaint(); //清除时钟画笔（如果有）
}
void MainWidget::do_btnLCD_clicked()
{
    //ui->btnLCD->clicked(true);
    do_displayTimer_timeout();
    ui->labTime->setHidden(true);
    ui->labDate->setHidden(true);
    ui->lcdTime->setHidden(false);
    ui->lcdDate->setHidden(false);
    settings.setValue("DateTimeStyle", 1);
    repaint(); //清除时钟画笔（如果有）
}
void MainWidget::do_btnDial_clicked()
{
    //ui->btnDial->clicked(true);
    ui->labTime->setHidden(true);
    ui->labDate->setHidden(true);
    ui->lcdTime->setHidden(true);
    ui->lcdDate->setHidden(true);
    do_displayTimer_timeout();
    settings.setValue("DateTimeStyle", 2);
}

void MainWidget::do_displayTimer_timeout()
{
    time = QTime::currentTime();
    date = QDate::currentDate();

    if(ui->btnNormal->isChecked())
    {
        ui->labTime->setText(time.toString("hh:mm:ss"));
        ui->labDate->setText(date.toString("yyyy.MM.dd"));
    }

    if(ui->btnLCD->isChecked())
    {
        ui->lcdTime->display(time.toString("hh:mm:ss"));
        ui->lcdDate->display(date.toString("yyyy.MM.dd"));
    }
    if(ui->btnDial->isChecked())
    {
        update();
    }
}

void MainWidget::do_refreshTimer_timeout()
{
    //刷新计时器
    displayTimer->setInterval(1);

    if(ui->btnLocal->isEnabled())
    {
        fetchInternetTime();
    }
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{//鼠标按键被按下
    if(isFullScreen()) do_btnFull_clicked();
    else
    {
        if (event->button() == Qt::LeftButton && !isFullScreen())
        {
            mouseIsMoving = true;
            mouseLastPositon = event->globalPos() - this->pos();
        }
        return QWidget::mousePressEvent(event);
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{//鼠标按下左键移动
    if(!isFullScreen())
    {
        QPoint eventPos=event->globalPos();

        if (mouseIsMoving && (event->buttons() & Qt::LeftButton)
            && (eventPos-mouseLastPositon).manhattanLength() > QApplication::startDragDistance())
        {
            move(eventPos-mouseLastPositon);
            mouseLastPositon = eventPos - this->pos();
        }
        return QWidget::mouseMoveEvent(event);
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{ //鼠标按键被释放
    if(!isFullScreen())
    {
        mouseIsMoving=false;     //停止移动
        event->accept();
    }
}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    if(isFullScreen()) do_btnFull_clicked();
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    //QStyleOption
    if(ui->btnDial->isChecked())
    {
        QPainter painter(this); //创建QPainter对象
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //设置画笔
        QPen pen;
        pen.setWidth(2);        //线宽
        pen.setColor(qRgb(0,0,0)); //划线颜色
        pen.setStyle(Qt::SolidLine);        //线的类型，实线、虚线等
        pen.setCapStyle(Qt::RoundCap);       //线端点样式
        pen.setJoinStyle(Qt::RoundJoin);    //线的连接点样式
        painter.setPen(pen);

        //设置画刷
        QBrush brush;
        brush.setColor(qRgb(255,255,255));         //画刷颜色
        brush.setStyle(Qt::SolidPattern);   //画刷填充样式
        painter.setBrush(brush);

        //设置字体
        QFont   font;
        font.setFamily("Arial");
        font.setPointSize(12);
        font.setBold(true);
        painter.setFont(font);

        int dialWidgh =width()-100;
        int dialHeight = height()-100;
        int side = qMin(dialWidgh,dialHeight);
        QRect rect((dialWidgh-side)/2+50, (dialHeight-side)/2+50, side, side);
        //painter.drawRect(rect);
        painter.setViewport(rect);
        painter.setWindow(-100,-100,200,200);

        painter.drawEllipse(QPoint(0,0),100,100);
        brush.setColor(qRgb(0,0,0));
        painter.drawEllipse(QPoint(0,0),2,2);

        for(int i=0; i<60; i++)
        {
            QLine lineHour(0,95,0,80);
            QLine lineMinute(0,95,0,85);
            painter.drawLine(i%5==0 ? lineHour : lineMinute);
            painter.rotate(6);
        }
        painter.save();
        painter.rotate((((time.hour())%12*5)+(time.minute()/12))*6);
        painter.drawLine(0,0,0,-30);
        painter.restore();

        painter.save();
        painter.rotate(time.minute()*6);
        painter.drawLine(0,0,0,-45);
        painter.restore();

        painter.save();
        painter.rotate(time.second()*6);
        painter.drawLine(0,0,0,-60);
        painter.restore();

        int angle = 90;
        qreal rotate = 30;
        qreal pi = M_PI;
        QPoint point(0,-75);
        QString number = "12";
        for(int i = 0; i < 12; i++)
        {
            if(i != 0) number.setNum(i);
            painter.drawText(point.x()-10, point.y()-10, 20, 20, Qt::AlignCenter, number);
            qreal arc = (rotate - angle) / 180 * pi;
            qreal length = qSqrt(point.x() * point.x() + point.y() * point.y());
            point.setX(length*qCos(arc));
            point.setY(length*qSin(arc));
            angle -= 30;
        }
    }
    event->accept();
}
