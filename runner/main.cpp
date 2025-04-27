/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include <config.h>

// qt
#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QIcon>
#include <QDesktopWidget>

// plexydesk
#include <ck_workspace.h>
#include <ck_config.h>
#include <ck_extension_manager.h>
#include <ck_resource_manager.h>
#include <ck_system_window_context.h>

#if defined(Q_OS_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#ifdef __QT5_TOOLKIT__
#include <qpa/qplatformnativeinterface.h>
#endif
#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>
#include <X11/Xutil.h>
#include <netwm.h>
#endif

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <tchar.h>
#include <clrdata.h>
#endif

#include "desktopmanager.h"
#include <ck_screen.h>

#ifdef Q_OS_WIN32
void configureWindowsWorkspace(DesktopManager *workspace, bool desktop_mode = false) {
    HWND hwnd = (HWND)workspace->winId();

    if (desktop_mode) {
        // Find Progman
        HWND progman = FindWindowW(L"Progman", NULL);
        if (!progman) {
            qWarning() << "Failed to find Progman window";
            return;
        }

        SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);

        // Find WorkerW
        HWND workerw = NULL;
        HWND shell_view = FindWindowExW(progman, NULL, L"SHELLDLL_DefView", NULL);
        if (shell_view) {
            workerw = FindWindowExW(NULL, progman, L"WorkerW", NULL);
        }

        if (!workerw) {
            qWarning() << "Failed to find WorkerW window";
            return;
        }

        // Set parent to WorkerW
        SetParent(hwnd, workerw);

        HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(hmon, &mi)) {
            RECT monitor_rect = mi.rcMonitor;
            // Set position relative to WorkerW
            SetWindowPos(hwnd, HWND_BOTTOM,
                         monitor_rect.left, monitor_rect.top,
                         monitor_rect.right - monitor_rect.left,
                         monitor_rect.bottom - monitor_rect.top,
                         SWP_NOACTIVATE | SWP_NOZORDER);
        } else {
            qWarning() << "Failed to get monitor info";
        }

        LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style |= WS_EX_NOACTIVATE | WS_EX_LAYERED;
        SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);

        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    } else {
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_BORDER);
        SetWindowLong(hwnd, GWL_STYLE, style);

        SetWindowPos(hwnd, HWND_TOP, 100, 100, 800, 600, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

void resetWindowsSessionLog() {
    QFile sync_file(QDir::toNativeSeparators(QDir::homePath() + "/plexydesk-session-log.txt"));
    sync_file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (sync_file.isOpen()) {
        QTextStream sync_stream(&sync_file);
        sync_stream << "";
        sync_file.close();
    }
}

void syncWindowsSessionLog(QtMsgType msg_type, const QMessageLogContext &ctx, const QString &data) {
    QByteArray debug_data = data.toLocal8Bit();
    QFile sync_file(QDir::toNativeSeparators(QDir::homePath() + "/plexydesk-session-log.txt"));
    sync_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    if (sync_file.isOpen()) {
        QTextStream sync_stream(&sync_file);
        if (msg_type == QtWarningMsg || msg_type == QtDebugMsg) {
            sync_stream << debug_data.constData() << " - " << endl;
        }
        sync_file.close();
    }
}
#endif

#ifdef Q_OS_LINUX
void configureLinuxWorkspace(DesktopManager *workspace, const char *platform_name) {
    if (platform_name && (strcmp(platform_name, "xcb") == 0)) {
        QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
        Display *display = static_cast<Display *>(native->nativeResourceForWindow("display", NULL));
        if (display) {
            NETWinInfo info(display, workspace->winId(), RootWindow(display, 0), NET::WMDesktop);
            info.setDesktop(NETWinInfo::OnAllDesktops);
            info.setWindowType(NET::Desktop);
        }
    }
}
#endif

class Runtime {
public:
    Runtime(const char *platform_name = nullptr, bool windowed_mode = false) {
        for (int i = 0; i < cherry_kit::screen::get()->screen_count(); i++) {
            DesktopManager *workspace = new DesktopManager();
            workspace->move_to_screen(i);
            m_workspace_list.push_back(workspace);

            workspace->add_default_controller("classicbackdrop");
            workspace->add_default_controller("dockwidget");
            workspace->add_default_controller("plexyclock");
            workspace->add_default_controller("desktopnoteswidget");
            workspace->add_default_controller("folderwidget");
            workspace->add_default_controller("datewidget");
            workspace->add_default_controller("photoframe");
            workspace->add_default_controller("desksyncwidget");

            workspace->restore_session();

            if (workspace->space_count() <= 0) {
                workspace->add_default_space();
            }

            workspace->expose(0);

            if (!windowed_mode) {
#ifdef Q_OS_LINUX
                configureLinuxWorkspace(workspace, platform_name);
#endif

#ifdef Q_OS_WIN32
                configureWindowsWorkspace(workspace);
#endif
            }

            workspace->show();
        }
    }

    ~Runtime() {
        for (auto workspace : m_workspace_list) {
            delete workspace;
        }
        m_workspace_list.clear();

        delete cherry_kit::resource_manager::instance();
        cherry_kit::extension_manager::instance()->destroy_instance();
    }

    std::vector<DesktopManager *> m_workspace_list;
};

Q_DECL_EXPORT int main(int argc, char *argv[]) {
#ifdef Q_OS_WIN32
    resetWindowsSessionLog();
    qInstallMessageHandler(syncWindowsSessionLog);
#endif

    char *platform_name = nullptr;
    bool windowed_mode = false;
    bool desktop_mode = false;

#ifdef Q_OS_LINUX
    for (int i = 0; i < argc; i++) {
        if ((strcmp(argv[i], "-platform") == 0) && (i + 1 < argc)) {
            platform_name = argv[i + 1];
            break;
        }
    }
#endif

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--windowed") == 0) {
            windowed_mode = true;
            break;
        }
        if (strcmp(argv[i], "--desktop") == 0) {
            desktop_mode = true;
            break;
        }
    }

    QApplication app(argc, argv);

    cherry_kit::extension_manager::instance(
        QDir::toNativeSeparators(cherry_kit::config::instance()->prefix() + QLatin1String("/share/plexy/ext/groups/")),
        QDir::toNativeSeparators(cherry_kit::config::instance()->prefix() + QLatin1String("/plexyext/"))
    );

    Runtime runtime(platform_name, windowed_mode);

#ifdef Q_OS_WIN32
    if (!windowed_mode) {
        for (auto workspace : runtime.m_workspace_list) {
            configureWindowsWorkspace(workspace, desktop_mode);
        }
    }
#endif

    return app.exec();
}
