#include "languagedialog.h"
#include "ui_languagedialog.h"

#include <QApplication>
#include <QTranslator>
#include <QFileInfo>

LanguageDialog::LanguageDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LanguageDialog)
{
    ui->setupUi(this);

    //如果语言为空字符串，表示使用系统默认语言
    if(language.isEmpty())
        ui->listWidget->setCurrentRow(0);
    
    // 创建语言映射表 - 用于显示母语+翻译语言
    QMap<QString, QString> languageMap;
    languageMap["en"] = "English (英文)";
    languageMap["zh_CN"] = "中文 (简体)";
    languageMap["zh_TW"] = "中文 (繁体)";
    
    // 首先添加英语选项（默认语言，不需要翻译文件）
    QListWidgetItem *englishItem = new QListWidgetItem(languageMap["en"], ui->listWidget);
    englishItem->setData(Qt::UserRole, "en");
    
    // 直接添加应用程序支持的所有语言，不依赖于翻译文件是否存在
    QStringList supportedLanguages;
    supportedLanguages << "zh_CN" << "zh_TW";
    
    foreach (const QString &languageCode, supportedLanguages) {
        // 获取显示名称
        QString displayName = languageMap[languageCode];
        
        // 添加到列表中
        QListWidgetItem *item = new QListWidgetItem(displayName, ui->listWidget);
        item->setData(Qt::UserRole, languageCode);
    }
}

LanguageDialog::~LanguageDialog()
{
    delete ui;
}

void LanguageDialog::saveLanguage()
{
    // 获取选中的语言
    QListWidgetItem *currentItem = ui->listWidget->currentItem();
    if (currentItem) {
        QString languageCode = currentItem->data(Qt::UserRole).toString();
        
        // 保存设置
        settings.setValue("Language", languageCode);
    }
}

void LanguageDialog::accept()
{
    //if(language != ui->listWidget->curr)
    QMessageBox::question(this, tr("Question"), tr("Want to restart the application to apply your new language?"));
}
