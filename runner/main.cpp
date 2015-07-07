/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <QGuiApplication>
#include <QtCore>
#include <QtGui>
#include <QIcon>
#include <QDesktopWidget>

// plexydesk
#include <plexy.h>
#include <workspace.h>
#include <plexyconfig.h>
#include <extensionmanager.h>

#if defined(Q_OS_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <qpa/qplatformnativeinterface.h>

#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>
#include <X11/Xutil.h>

#include <netwm.h>
#endif

#include "desktopmanager.h"

#ifdef Q_OS_WIN
// Windows
#include <Windows.h>
#include <tchar.h>

WNDPROC wpOrigEditProc;

// Subclass procedure
LRESULT APIENTRY
EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_WINDOWPOSCHANGING) {

    if (!SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                      SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE)) {
      qDebug() << Q_FUNC_INFO << "Error maintaining native z order";
    }
    return 0;
  }

  if (uMsg == WM_WINDOWPOSCHANGED) {
    /*
    if (!SetWindowPos( hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER
    |
    SWP_NOACTIVATE)) {
            qDebug() << Q_FUNC_INFO << "Error maintaining native z order";
    }
    */

    return 0;
  }

  if (uMsg == WM_SETFOCUS) {
    /*
    if (!SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE |
    SWP_NOACTIVATE)) {
            qDebug() << Q_FUNC_INFO << "Error changing native z order";
    }
    */

    return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
  }

  return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
}

// usage
//     CHAR msgText[256];
//     getLastErrorText(msgText,sizeof(msgText));
static CHAR *          //   return error message
    getLastErrorText(  // converts "Lasr Error" code into text
        CHAR *pBuf,    //   message buffer
        ULONG bufSize) //   buffer size
{
  DWORD retSize;
  LPTSTR pTemp = NULL;

  if (bufSize < 16) {
    if (bufSize > 0) {
      pBuf[0] = '\0';
    }
    return (pBuf);
  }
  retSize = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_ARGUMENT_ARRAY,
      NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR) & pTemp, 0, NULL);
  if (!retSize || pTemp == NULL) {
    pBuf[0] = '\0';
  } else {
    pTemp[strlen(pTemp) - 2] = '\0'; // remove cr and newline character
    sprintf(pBuf, "%0.*s (0x%x)", bufSize - 16, pTemp, GetLastError());
    LocalFree((HLOCAL)pTemp);
  }
  return (pBuf);
}
#endif

class Runtime {
public:
  Runtime(const char *a_platform_name = 0) {
    for (int i = 0; i < qApp->desktop()->screenCount(); i++) {
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

      workspace->restore_session();

      if (workspace->space_count() <= 0) {
        workspace->add_default_space();
      }

#ifdef Q_OS_LINUX
      QPlatformNativeInterface *native =
          QGuiApplication::platformNativeInterface();

      if (native && a_platform_name && (strcmp(a_platform_name, "xcb") == 0)) {
        Display *display = static_cast<Display *>(
            native->nativeResourceForWindow("display", NULL));
        if (display) {
          NETWinInfo info(display, workspace->winId(), RootWindow(display, 0),
                          NET::WMDesktop);
          info.setDesktop(NETWinInfo::OnAllDesktops);
          info.setWindowType(NET::Desktop);
        } 
        // handle wayland
      }
#endif
      workspace->show();

#ifdef Q_OS_WIN
      // HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
      HWND hShellWnd = GetShellWindow();
      HWND hDefView =
          FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
      HWND folderView = FindWindowEx(hDefView, NULL, _T("SysListView32"), NULL);

      SetParent((HWND)workspace->winId(), folderView);
// SetWindowPos((HWND) workspace->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE
// |
// SWP_NOSIZE | SWP_NOACTIVATE);
// SetWindowPos((HWND) workspace->winId(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE |
// SWP_NOSIZE | SWP_NOACTIVATE);
// wpOrigEditProc = (WNDPROC) SetWindowLong((HWND) workspace->winId(),
// GWL_WNDPROC, (LONG) EditSubclassProc);
#endif
    }
  }

  ~Runtime() {
    std::for_each(std::begin(m_workspace_list), std::end(m_workspace_list),
                  [&](DesktopManager *a_manager) { delete a_manager; });

    m_workspace_list.clear();
  }

private:
  std::vector<DesktopManager *> m_workspace_list;
};

Q_DECL_EXPORT int main(int argc, char *argv[]) {
  char *runtime_platform_name = 0;
#ifdef Q_OS_LINUX
  for (int i = 0; i < argc; i++) {
    if ((strcmp(argv[i], "-platform") != 0) ||
        (argc < (i + 1) && strlen(argv[i + 1]) <= 0) || (argv[i + 1][0] == '-'))
      continue;

    runtime_platform_name = argv[i + 1];
  }

  if (!runtime_platform_name) {
    const char *null_platform = "xcb";
    runtime_platform_name = (char *)malloc(sizeof(null_platform));
    strncpy(runtime_platform_name, null_platform, sizeof(null_platform));
  }
  printf("Detected Platform %s\n", runtime_platform_name);
#endif

  QApplication app(argc, argv);
  CherryKit::ExtensionManager *loader = 0;

  loader = CherryKit::ExtensionManager::instance(
      QDir::toNativeSeparators(CherryKit::Config::instance()->prefix() +
                               QLatin1String("/share/plexy/ext/groups/")),
      QDir::toNativeSeparators(CherryKit::Config::instance()->prefix() +
                               QLatin1String("/lib/plexyext/")));

#ifndef Q_WS_QPA
  QString appIconPath =
      CherryKit::Config::instance()->prefix() + "/share/plexy/plexydesk.png";
  QIcon appIcon = QIcon(QDir::toNativeSeparators(appIconPath));
  app.setWindowIcon(appIcon);
  app.setApplicationName(QString(PLEXYNAME));
  QApplication::setQuitOnLastWindowClosed(true);
#endif

  Runtime runtime(runtime_platform_name);

  return app.exec();
}
