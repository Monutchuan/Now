#include "mainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,500);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    //ui->HLayMode->setHidden(true);
    displayTimer = new QTimer(this); //用于显示时间
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(do_displayTimer_timeout()));
    displayTimer->start(1000);

    refreshTimer = new QTimer(this); //用于更新时间
    refreshTimer->stop();

    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(do_refreshTimer_timeout()));

    if(!settings.value("UseInternetTime", false).toBool())
    {
        ui->btnLocal->setChecked(true);
        on_btnLocal_clicked();
    }
    else
    {
        ui->btnInternet->setChecked(true);
        on_btnInternet_clicked();
    }

    switch(settings.value("DateTimeStyle", 0).toInt())
    {
    case 0:
        ui->btnNormal->setChecked(true);
        on_btnNormal_clicked();
        break;
    case 1:
        ui->btnLCD->setChecked(true);
        on_btnLCD_clicked();
        break;
    case 2:
        ui->btnDial->setChecked(true);
        on_btnDial_clicked();
    }

    QMenu *menu = new QMenu(this);

    QSettings launch("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    ui->actLaunch->setChecked(launch.contains(qAppName()));

    ui->actBell->setChecked(settings.value("Bell", false).toBool());

    menu->addAction(ui->actLaunch);
    menu->addSeparator();
    menu->addAction(ui->actRefresh);
    menu->addAction(ui->actServer);
    menu->addSeparator();
    menu->addAction(ui->actBell);
    menu->addSeparator();
    menu->addAction(ui->actLanguage);
    menu->addAction(ui->actAbout);
    ui->btnMenu->setMenu(menu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fetchInternetTime()
{
    QUdpSocket* udpSocket = new QUdpSocket(this);
    QByteArray request(48, 0);
    request[0] = 0x1B; //NTP请求头

    QString ntpServer = "time.windows.com";
    int ntpPort = 123;

    connect(udpSocket, &QUdpSocket::readyRead, this, [=]()
    {
        while (udpSocket->hasPendingDatagrams())
        {
            QByteArray buffer;
            buffer.resize(udpSocket->pendingDatagramSize());
            udpSocket->readDatagram(buffer.data(), buffer.size());

            if (buffer.size() >= 48)
            {
                //NTP 时间是从 1900 年开始的秒数
                quint32 secondsSince1900;
                memcpy(&secondsSince1900, buffer.data() + 40, 4);
                secondsSince1900 = qFromBigEndian(secondsSince1900);

                //转换为 UNIX 时间戳（从 1970 开始）
                const quint32 seventyYears = 2208988800U;
                quint32 unixTime = secondsSince1900 - seventyYears;

                QDateTime dateTime = QDateTime::fromSecsSinceEpoch(unixTime);
                dateTime = dateTime.toLocalTime(); //转换为本地时间

                //设置到变量
                time = dateTime.time();
                date = dateTime.date();

                //调用成功回调
                ui->labInternet->setText(QString(tr("Successfully Obtained Network Time: %1, %2, every ten minutes")).arg(date.toString()).arg(time.toString()));

                udpSocket->deleteLater();
            }
        }
    });

    //获取 NTP 服务器 IP 地址并发送请求
    QHostInfo::lookupHost(ntpServer, this, [=](const QHostInfo &info)
    {
        if (!info.addresses().isEmpty())
        {
            QHostAddress address = info.addresses().first();
            udpSocket->writeDatagram(request, address, ntpPort);
        }
        else
        {
            ui->labInternet->setText(QString(tr("Unable To Resolve Host: %1, Please Use Local Time")).arg(ntpServer));
            udpSocket->deleteLater();
        }
    });
}

void MainWindow::do_displayTimer_timeout()
{
    if(ui->btnLocal->isChecked())
    {//本地时间
        time = QTime::currentTime();
        date = QDate::currentDate();
    }
    else
    {//网络时间
        time = time.addSecs(1);
        if(time == QTime(23,59,59))
            date = date.addDays(1);
    }

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

void MainWindow::do_refreshTimer_timeout()
{
    //刷新计时器
    refreshTimer->setInterval(1000);

    if(ui->btnLocal->isEnabled())
    {
        fetchInternetTime();
    }
}

void MainWindow::on_btnMenu_clicked()
{
    ui->btnMenu->setMenu(new QMenu(this));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); //创建QPainter对象
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(QColor(0, 0, 0, 128)); //透明黑色

    QRect backgroundRect = rect();
    painter.drawRect(backgroundRect);

    if (ui->btnDial->isChecked())
    {
        //===== 关键修复：重新计算坐标变换 =====
        int dialWidth = width() - 120;
        int dialHeight = height() - 120;
        int side = qMin(dialWidth, dialHeight);

        //计算居中矩形（修正原代码中的偏移问题）
        QRect viewportRect((width() - side) / 2,  //X坐标居中计算
                           (height() - side) / 2, //Y坐标居中计算
                           side, side);

        painter.setViewport(viewportRect); //视口：物理坐标系中的可见区域
        painter.setWindow(-100, -100, 200, 200); //窗口：逻辑坐标系（保持1:1比例）

        //创建渐变背景
        QRadialGradient gradient(0, 0, 100);
        gradient.setColorAt(0.0, QColor(240, 240, 240)); //中心亮色
        gradient.setColorAt(0.8, QColor(200, 200, 200)); //中间过渡
        gradient.setColorAt(1.0, QColor(160, 160, 160)); //边缘深色
        painter.setBrush(gradient);

        //绘制带阴影的表盘
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPoint(0, 0), 100, 100);

        //添加金属边框
        QPen borderPen(QColor(80, 80, 80), 3);
        painter.setPen(borderPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPoint(0, 0), 100, 100);

        //立体刻度线
        for (int i = 0; i < 60; i++)
        {
            if (i % 5 == 0)
            {
                //小时刻度（更粗更长）
                QPen hourPen(QColor(50, 50, 50), 3, Qt::SolidLine, Qt::RoundCap);
                painter.setPen(hourPen);
                painter.drawLine(0, 95, 0, 80);

            }
            else
            {
                //分钟刻度（更细）
                QPen minutePen(QColor(100, 100, 100), 1, Qt::SolidLine, Qt::RoundCap);
                painter.setPen(minutePen);
                painter.drawLine(0, 95, 0, 85);
            }
            painter.rotate(6);
        }

        //中心轴
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawEllipse(QPoint(0, 0), 5, 5);
        painter.setBrush(Qt::darkGray);
        painter.drawEllipse(QPoint(0, 0), 3, 3);

        //时针（钛银渐变，尖端收窄）
        QLinearGradient hourGrad(0, 40, 0, -40);         //拉长渐变范围
        hourGrad.setColorAt(0, QColor(180, 180, 190));   //底部：金属银
        hourGrad.setColorAt(0.7, QColor(100, 100, 120)); //中部：钛灰色
        hourGrad.setColorAt(1, QColor(60, 60, 80));      //尖端：深灰
        painter.setBrush(hourGrad);
        painter.save();
        painter.rotate((((time.hour()) % 12 * 5) + (time.minute() / 12)) * 6);

        QPainterPath hourPath;
        hourPath.moveTo(-2.5, 25); //底部稍宽
        hourPath.lineTo(2.5, 25);
        hourPath.lineTo(1.2, -38); //尖端收窄（原-30改为-38，长度增加26%）
        hourPath.lineTo(-1.2, -38);
        hourPath.closeSubpath();
        painter.drawPath(hourPath);
        painter.restore();

        //分针（科技蓝渐变，保持最长）
        QLinearGradient minGrad(0, 50, 0, -50);
        minGrad.setColorAt(0, QColor(100, 180, 255)); //底部：霓虹蓝
        minGrad.setColorAt(1, QColor(20, 80, 160));   //尖端：深科技蓝
        painter.setBrush(minGrad);
        painter.save();
        painter.rotate(time.minute() * 6);

        QPainterPath minutePath;
        minutePath.moveTo(-2, 30);
        minutePath.lineTo(2, 30);
        minutePath.lineTo(1, -50); //保持比分针短
        minutePath.lineTo(-1, -50);
        minutePath.closeSubpath();
        painter.drawPath(minutePath);
        painter.restore();

        //秒针（能量红渐变+箭头）
        QLinearGradient secGrad(0, 20, 0, -65);
        secGrad.setColorAt(0, QColor(255, 80, 80)); //根部：亮红
        secGrad.setColorAt(1, QColor(180, 0, 0));   //尖端：深红

        painter.setPen(Qt::NoPen); //取消边框线
        painter.setBrush(secGrad);
        painter.save();
        painter.rotate(time.second() * 6);

        //流线型秒针设计
        QPainterPath secondPath;
        secondPath.moveTo(0, 15);
        secondPath.lineTo(1.5, 15);
        secondPath.lineTo(0.8, -65); //超细长设计
        secondPath.lineTo(-0.8, -65);
        secondPath.lineTo(-1.5, 15);
        secondPath.closeSubpath();

        painter.drawPath(secondPath);
        painter.restore();

        //数字的绘制
        QFont font("Arial", 12, QFont::Bold);
        font.setStyleStrategy(QFont::PreferAntialias);
        painter.setFont(font);

        int angle = 90;
        qreal rotate = 30;
        QPoint point(0, -70); //第一个数字“12”的位置
        for (int i = 0; i < 12; i++)
        {
            QString number = QString::number(i == 0 ? 12 : i);

            //数字文本
            painter.setPen(Qt::black);
            painter.drawText(point.x() - 10, point.y() - 10, 20, 20, Qt::AlignCenter, number);

            //数字位置
            qreal arc = qDegreesToRadians(rotate - angle);
            qreal length = 70;
            point.setX(length * qCos(arc));
            point.setY(length * qSin(arc));
            angle -= 30;
        }
    }
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (isFullScreen())
        on_btnFull_clicked(); //处理全屏逻辑
    else if (event->button() == Qt::LeftButton)
        mouseLastPositon = event->globalPos() - this->pos();
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!isFullScreen() && (event->buttons() & Qt::LeftButton))
    {
        QPoint eventPos = event->globalPos();
        if ((eventPos - mouseLastPositon - this->pos()).manhattanLength() > QApplication::startDragDistance())
        {
            move(eventPos - mouseLastPositon);
            mouseLastPositon = eventPos - this->pos(); //更新位置
        }
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isFullScreen())
        event->accept();
    QWidget::mouseReleaseEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    if(isFullScreen()) on_btnFull_clicked();
}

void MainWindow::on_btnNormal_clicked()
{
    do_displayTimer_timeout();
    ui->lcdTime->setHidden(true);
    ui->lcdDate->setHidden(true);
    ui->labTime->setHidden(false);
    ui->labDate->setHidden(false);
    settings.setValue("DateTimeStyle", 0);
    repaint(); //清除时钟画笔（如果有）
}


void MainWindow::on_btnLCD_clicked()
{
    do_displayTimer_timeout();
    ui->labTime->setHidden(true);
    ui->labDate->setHidden(true);
    ui->lcdTime->setHidden(false);
    ui->lcdDate->setHidden(false);
    settings.setValue("DateTimeStyle", 1);
    repaint(); //清除时钟画笔（如果有）
}


void MainWindow::on_btnDial_clicked()
{
    ui->labTime->setHidden(true);
    ui->labDate->setHidden(true);
    ui->lcdTime->setHidden(true);
    ui->lcdDate->setHidden(true);
    do_displayTimer_timeout();
    settings.setValue("DateTimeStyle", 2);
}


void MainWindow::on_actLaunch_triggered(bool checked)
{
    QSettings launch("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                     QSettings::NativeFormat);
    QString path = "\"" + QCoreApplication::applicationFilePath().replace('/', '\\') + "\"";

    if (checked)
        launch.setValue(qAppName(), path);
    else
        launch.remove(qAppName());
}

void MainWindow::on_btnLocal_clicked()
{//按下“本地”按钮
    ui->labInternet->clear();
    refreshTimer->stop();
    settings.setValue("UseInternetTime", false);
}


void MainWindow::on_btnInternet_clicked()
{
    fetchInternetTime();
    refreshTimer->start(20); //十分钟
    settings.setValue("UseInternetTime", true);
}

void MainWindow::on_btnFull_clicked()
{//按下“全屏”按钮
    if(isFullScreen())
    {
        showNormal();

        ui->HLayTitle->setHidden(false);
        ui->HLayStyle->setHidden(false);
        ui->HLayMode->setHidden(false);

        QFont font;
        font = ui->labTime->font();
        font.setPointSize(font.pointSize()/2);
        ui->labTime->setFont(font);

        font = ui->labDate->font();
        font.setPointSize(font.pointSize()/2);
        ui->labDate->setFont(font);

        ui->lcdTime->setMinimumHeight(160);
        ui->lcdDate->setMinimumHeight(110);

        update();
    }
    else
    {
        showFullScreen();

        ui->HLayTitle->setHidden(true);
        ui->HLayStyle->setHidden(true);
        ui->HLayMode->setHidden(true);

        QFont font;
        font = ui->labTime->font();
        font.setPointSize(font.pointSize()*2);
        ui->labTime->setFont(font);

        font = ui->labDate->font();
        font.setPointSize(font.pointSize()*2);
        ui->labDate->setFont(font);

        ui->lcdTime->setMinimumHeight(320);
        ui->lcdDate->setMinimumHeight(220);

        update();
    }
}

void MainWindow::on_actRefresh_triggered()
{
    if(!ui->btnInternet->isChecked())
        QMessageBox::critical(this, tr("Critical"), tr("Only for Internet mode!"));
    else
        fetchInternetTime();
}


void MainWindow::on_actServer_triggered()
{
    serverDialog = new ServerDialog(this);
    serverDialog->show();
}


void MainWindow::on_actBell_triggered(bool checked)
{
    settings.setValue("Bell", checked);
}


void MainWindow::on_actLanguage_triggered()
{
    languageDialog = new LanguageDialog(this);
    languageDialog->show();
}


void MainWindow::on_actAbout_triggered()
{
    QMessageBox::about(this, tr("About"), tr("MTC Now\n"));
}

