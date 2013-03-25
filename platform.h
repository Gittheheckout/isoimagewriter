#ifndef PLATFORM_H
#define PLATFORM_H

#include <QtGlobal>

#if defined(Q_OS_WIN32)
#include <windows.h>
#include <dbt.h>
#include <Wbemidl.h>
#include <Shobjidl.h>
#include <comutil.h>
#include <io.h>
#endif

#if defined(Q_OS_LINUX)
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <libudev.h>
#endif


#endif // PLATFORM_H