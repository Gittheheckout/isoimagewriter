#include <QApplication>
#include <QTranslator>

#include "common.h"
#include "maindialog.h"
#include "usbdevicemonitor.h"

#if !defined(Q_OS_WIN32) && !defined(Q_OS_LINUX)
#error Unsupported platrofm!
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator appTranslator;
    appTranslator.load(QLocale::system().name(), QCoreApplication::applicationDirPath() + "/lang");
    a.installTranslator(&appTranslator);

#if defined(Q_OS_WIN32)
    // CoInitialize() seems to be called by Qt automatically, so only set security attributes
    HRESULT res = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0);
    if (res != S_OK)
    {
        printf("CoInitializeSecurity failed! (Code: 0x%08lx)\n", res);
        return res;
    }
#endif

    MainDialog w;
    w.show();

    UsbDeviceMonitor deviceMonitor;
    deviceMonitor.startMonitoring();

    // When device changing event comes, refresh the list of USB flash disks
    // Using QueuedConnection to avoid delays in processing the message
    QObject::connect(&deviceMonitor, &UsbDeviceMonitor::deviceChanged, &w, &MainDialog::scheduleEnumFlashDevices, Qt::QueuedConnection);

    return a.exec();
}
