#ifndef LANGUAGEDIALOG_H
#define LANGUAGEDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QLocale>
#include <QMessageBox>

namespace Ui
{
class LanguageDialog;
}

class LanguageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageDialog(QWidget *parent = nullptr);
    ~LanguageDialog();

private:
    Ui::LanguageDialog *ui;
    QSettings settings;
    QLocale locale = QLocale::system();
    QString language = settings.value("Language", locale.name()).toString();
    
private slots:
    void saveLanguage();

protected:
    void accept();
};

#endif // LANGUAGEDIALOG_H
