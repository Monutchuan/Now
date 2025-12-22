#ifndef NTPDIALOG_H
#define NTPDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>

#include <QUdpSocket>
#include <QHostInfo>

#include <QMessageBox>

namespace Ui
{
class NTPDialog;
}

class NTPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NTPDialog(QWidget *parent = nullptr, QListWidgetItem *item = nullptr);
    ~NTPDialog();

private:
    Ui::NTPDialog *ui;
    QSettings settings;
    QUdpSocket *udpSocket;

protected:
    void accept();
};

#endif // NTPDIALOG_H
