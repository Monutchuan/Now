#include "mainwidget.h"

#include <QApplication>
#include <QSharedMemory>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QSplashScreen>

QTranslator translator;
QSettings settings;

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

// 激活窗口的跨平台方法
void activateWindowByHandle(qintptr windowHandle)
{
#ifdef Q_OS_WINDOWS
    // 在 Windows 上，通过窗口句柄激活窗口
    HWND hwnd = reinterpret_cast<HWND>(windowHandle);
    if (hwnd) {
        SetForegroundWindow(hwnd);
        ShowWindow(hwnd, SW_RESTORE); // 确保窗口从最小化状态恢复
    }
#else
    // 在其他平台，可以使用相应的窗口管理器接口（如 X11 或 Wayland）
    Q_UNUSED(windowHandle);
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("MTC");
    QApplication::setApplicationName("Now");

    const QString sharedMemoryKey = "MyUniqueAppKey";
    QSharedMemory sharedMemory(sharedMemoryKey);

    if (!sharedMemory.create(sizeof(qintptr))) {
        // 如果共享内存已存在，读取窗口句柄
        if (sharedMemory.attach()) {
            sharedMemory.lock();
            qintptr windowHandle;
            memcpy(&windowHandle, sharedMemory.constData(), sizeof(qintptr));
            sharedMemory.unlock();

            // 激活已有实例窗口
            activateWindowByHandle(windowHandle);
        }
        return 0; // 退出新实例
    }

    bool success = false;
    QLocale locale = QLocale::system();
    QString language = settings.value("Language", locale.name()).toString();
    QString file = QString(":/translation/MTC_Now_%1.qm").arg(language);
    success = translator.load(file);
    if(success) a.installTranslator(&translator);

    QSplashScreen splash(QPixmap(":/resource/splash.png"));
    splash.show();
    a.processEvents();
    MainWidget w;
    w.show();
    splash.finish(&w);
    return a.exec();

    // 将窗口句柄写入共享内存
    void* windowHandle = reinterpret_cast<void*>(w.winId());
    sharedMemory.lock();
    memcpy(sharedMemory.data(), &windowHandle, sizeof(qintptr));
    sharedMemory.unlock();
}
