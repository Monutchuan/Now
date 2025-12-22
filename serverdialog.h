#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include "ntpdialog.h"

#include <QDialog>
#include <QSettings>

namespace Ui
{
class ServerDialog;
}

class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = nullptr);
    ~ServerDialog();

private:
    Ui::ServerDialog *ui;
    NTPDialog *ntpDislog;
    QSettings settings;

protected:
    void accept();

private slots:
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnRemove_clicked();
    void on_listWidget_currentRowChanged(int currentRow);
};

#endif // SERVERDIALOG_H
