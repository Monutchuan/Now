#include "ntpdialog.h"
#include "ui_ntpdialog.h"

NTPDialog::NTPDialog(QWidget *parent, QListWidgetItem *item)
    : QDialog(parent)
    , ui(new Ui::NTPDialog)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);

    if(item != nullptr)
        ui->lineEdit->setText(item->text());
}

NTPDialog::~NTPDialog()
{
    delete ui;
}

void NTPDialog::accept()
{
    QHostInfo::lookupHost(ui->lineEdit->text(), this, [=](const QHostInfo &info)
    {
        QByteArray request(48, 0);
        request[0] = 0x1B; //NTPs请求头

        if (!info.addresses().isEmpty())
        {
            settings.setValue("NTPLink", ui->lineEdit->text());
            QMessageBox::information(this, tr("Success"), tr("%1\nOutput successful! Saved successfully.").arg(ui->lineEdit->text()));
        }
        else
        {
            udpSocket->deleteLater();
            QMessageBox::critical(this, tr("Critical"), tr("Output failed! Please try again later or use a different address."));
        }
    });
}
