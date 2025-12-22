#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QSplashScreen>
#include <QSharedMemory>
#include <QSystemSemaphore>

#ifdef Q_OS_WIN
#include <windows.h>

struct SharedData
{
    DWORD processId;
    HWND windowHandle;
};

#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
typedef Window SharedData;
#elif defined(Q_OS_MACOS)
#include <AppKit/AppKit.h>
typedef pid_t SharedData;
#endif

int main(int argc, char *argv[])
{
    QSystemSemaphore semaphore("MTC_Now_Semaphore", 1, QSystemSemaphore::Open);
    semaphore.acquire();

    QSharedMemory instanceCheck("MTC_Now_Instance_Check");
    bool isRunning = false;

    if (!instanceCheck.create(1))
    {
        if (instanceCheck.attach())
        {
            instanceCheck.detach();
            if (!instanceCheck.create(1))
                isRunning = true;
        }
        else
            isRunning = true;
    }

    if (isRunning)
    {
        QSharedMemory windowHandleMem("MTC_Now_Window_Handle");
        if (windowHandleMem.attach(QSharedMemory::ReadOnly))
        {
            windowHandleMem.lock();
            SharedData data;
            memcpy(&data, windowHandleMem.constData(), sizeof(SharedData));

#ifdef Q_OS_WIN
            if (IsWindow(data.windowHandle))
            {
                AllowSetForegroundWindow(data.processId);
                ShowWindow(data.windowHandle, SW_RESTORE);
                SetForegroundWindow(data.windowHandle);
            }
#elif defined(Q_OS_LINUX)
            Display* display = XOpenDisplay(nullptr);
            if (display && data)
            {
                XRaiseWindow(display, data);
                XSetInputFocus(display, data, RevertToParent, CurrentTime);
                XFlush(display);
                XCloseDisplay(display);
            }
#elif defined(Q_OS_MACOS)
            NSRunningApplication* app = [NSRunningApplication runningApplicationWithProcessIdentifier:data];
            [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
#endif
            windowHandleMem.unlock();
            windowHandleMem.detach();
        }
        semaphore.release();
        return 0;
    }

    semaphore.release();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

    QApplication a(argc, argv);
    QApplication::setOrganizationName("MTC");
    QApplication::setApplicationName("Now");

    //翻译
    bool success = false;
    QLocale locale = QLocale::system();
    QSettings settings;
    QString language = settings.value("Language", locale.name()).toString();
    
    //如果语言为空字符串，表示使用系统默认语言
    if(language.isEmpty())
        language = locale.name();
    
    QString file = QString(":/translation/MTC_Now_%1.qm").arg(language);
    QTranslator translator;
    success = translator.load(file);

    if(success)
        a.installTranslator(&translator);

    //启动界面
    QSplashScreen splash(QPixmap(":/resource/splash.png"));
    splash.setFont(QFont("Arial", 18, QFont::Bold));
    splash.showMessage("Now V1.0.0\nCopyright MTC(TEAM) All Rights Reserved", Qt::AlignCenter|Qt::AlignBottom, Qt::black);
    splash.show();

    MainWindow w;
    w.show();

    semaphore.acquire();
    QSharedMemory windowHandleMem("MTC_Now_Window_Handle");
    if (windowHandleMem.create(sizeof(SharedData)))
    {
        windowHandleMem.lock();
        SharedData data;

#ifdef Q_OS_WIN
        data.processId = GetCurrentProcessId();
        data.windowHandle = (HWND)w.winId();
#elif defined(Q_OS_LINUX)
        data = w.winId();
#elif defined(Q_OS_MACOS)
        data = getpid();
#endif

        memcpy(windowHandleMem.data(), &data, sizeof(SharedData));
        windowHandleMem.unlock();
    }
    semaphore.release();

    splash.finish(&w);
    return a.exec();
}
