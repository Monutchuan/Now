#include "serverdialog.h"
#include "ui_serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
    ui->comboBox->setCurrentText(settings.value("NTPLink","pool.ntp.org").toString()); //从注册表加载主要NTP地址
    ui->spinBox->setValue(settings.value("RefreshInterval",10).toInt()); //从注册表加载刷新间隔
    ui->listWidget->addItems(settings.value("NTPList").toStringList()); //从注册表加载备用NTP地址列表
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

void ServerDialog::on_btnAdd_clicked()
{
    ntpDislog = new NTPDialog(this);
    ntpDislog->show();
}


void ServerDialog::on_btnEdit_clicked()
{
    ntpDislog = new NTPDialog(this, ui->listWidget->currentItem());
    ntpDislog->show();
}


void ServerDialog::on_btnRemove_clicked()
{

}

void ServerDialog::accept()
{//如果用户点击了确定，那么将会把一些数据保存到注册表
    QStringList ntpList;
    for (int i = 0; i < ui->listWidget->count(); ++i) //将NTP地址列表先导入到一个QStringList变量中，然后将这个变量保存到注册表
        ntpList.append(ui->listWidget->item(i)->text());

    settings.setValue("NTPLink", ui->comboBox->currentText());
    settings.setValue("RefreshInterval", ui->spinBox->text());
    settings.setValue("NTPList", ntpList);
}

void ServerDialog::on_listWidget_currentRowChanged(int currentRow)
{
    ui->btnEdit->setEnabled(currentRow != -1);
    ui->btnRemove->setEnabled(currentRow != -1);
}

