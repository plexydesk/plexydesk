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
#include <QGuiApplication>
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
#include <qpa/qplatformnativeinterface.h>

#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>
#include <X11/Xutil.h>

#include <netwm.h>
#endif

#include "desktopmanager.h"
#include <ck_screen.h>

#ifdef Q_OS_WIN32
// Windows
#include <Windows.h>
#include <tchar.h>
#include <clrdata.h>

WNDPROC wpOrigEditProc;

extern void __reset_session_log();
extern void __sync_session_log(QtMsgType msg_type,
                        const QMessageLogContext &ctx,
                        const QString &data);


void __sync_session_log(QtMsgType msg_type,
                        const QMessageLogContext &ctx,
                        const QString &data)
{
    QByteArray debug_data = data.toLocal8Bit();

    QFile sync_file(
                QDir::toNativeSeparators(
                    QDir::homePath() + "/plexydesk-session-log.txt"));

    sync_file.open(QIODevice::WriteOnly
                        | QIODevice::Text
                        | QIODevice::Append);

    if (!sync_file.isOpen()) { return;}

    QTextStream sync_stream(&sync_file);

    if (msg_type == QtWarningMsg || msg_type == QtDebugMsg) {
      sync_stream <<  debug_data.constData() << " - " << endl;
    }

    sync_file.close();
}

void __reset_session_log() {
    QFile sync_file(
                QDir::toNativeSeparators(
                    QDir::homePath() + "/plexydesk-session-log.txt"));

    sync_file.open(QIODevice::WriteOnly | QIODevice::Text);

    if (!sync_file.isOpen()){ return;}

    QTextStream sync_stream(&sync_file);

    sync_stream << "";
    sync_file.close();
}

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
private:
#ifdef Q_OS_WIN32
    int check_windows_version() {
       OSVERSIONINFO osvi;
       ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
       osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
       GetVersionEx(&osvi);
       return osvi.dwMajorVersion;
    }

#endif

#ifdef Q_OS_WIN32
//#error "Not supported";
#endif
public:
  Runtime(const char *a_platform_name = 0) {
    for (int i = 0; i < cherry_kit::screen::get()->screen_count() ; i++) {
      DesktopManager *workspace = new DesktopManager();
      //workspace->set_accelerated_rendering(true);

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


#ifdef Q_OS_WIN32
      typedef HWND window_handle_t;

      /*
      window_handle_t desktop_window = GetShellWindow();

      if (desktop_window) {
          if (!SetParent((window_handle_t) workspace->winId(), desktop_window)) {
            qDebug() << Q_FUNC_INFO << "Error Reparengint...........";
            qApp->quit();
          } else {
            qDebug() << Q_FUNC_INFO << "-----Loading Windows desktop ---------";
          }
      } else {
          qDebug() << Q_FUNC_INFO << "-------Error Loading desktop ---------";
          qApp->quit();
      }
      */

      //find the Shell Window
      window_handle_t shell_window = FindWindowEx(GetShellWindow(),
                                                  NULL,
                                                  _T("SHELLDLL_DefView"),
                                                  NULL);
      qDebug() << Q_FUNC_INFO << "Shell Window" << shell_window;
      qDebug() << Q_FUNC_INFO << "Shell Window" << GetShellWindow();

      cherry_kit::device_window *d_window =
              cherry_kit::system_window_context::get()->find_window(
                  "FolderView", "SysListView32");
      qDebug() << Q_FUNC_INFO << "WIndow ID Found : " << d_window->handle();
      SetParent((window_handle_t)workspace->winId(), d_window->handle());
#endif

#ifdef Q_OS_WIN32_NON
      HWND hShellWnd = GetShellWindow();
      HWND hDefView =
          FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
      HWND folderView = FindWindowEx(hDefView, NULL, _T("SysListView32"), NULL);
      HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
      //HWND aero_shell_hwnd = FindWindow("CabinetWClass", "Personalization");

      if (!folderView)
          qApp->quit();

      if (check_windows_version() < 5){
         if (SetParent((HWND)workspace->winId(), folderView) == NULL)
          qApp->quit();
         return;
      } else {
         if (SetParent((HWND)workspace->winId(), hDefView) == NULL)
          qApp->quit();
      }

      LONG lStyle = GetWindowLong((HWND) workspace->winId(), GWL_STYLE);
      LONG current_window_ex_style = GetWindowLong((HWND) workspace->winId(), GWL_EXSTYLE);

      //lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
      lStyle &= ~(WS_SYSMENU | WS_MINIMIZE | WS_CAPTION | WS_SIZEBOX);
      lStyle |=
              (WS_MAXIMIZE
               | WS_CHILDWINDOW
               | WS_VISIBLE
               | WS_CLIPCHILDREN
               | WS_CLIPSIBLINGS
               );
      SetWindowLongPtr((HWND) workspace->winId(), GWL_STYLE, lStyle);

      current_window_ex_style &= ~(WS_EX_WINDOWEDGE
                                   | WS_EX_RIGHTSCROLLBAR
                                   | WS_EX_LEFTSCROLLBAR
                                   | WS_EX_LEFT
                                   | WS_EX_RIGHT
                                   | WS_EX_STATICEDGE
                                   );

      SetWindowLongPtr((HWND) workspace->winId(), GWL_EXSTYLE,
                       current_window_ex_style
                       | WS_EX_TOPMOST
                       | WS_EX_COMPOSITED
                       | WS_EX_TRANSPARENT
                       | WS_EX_ACCEPTFILES
                       | WS_EX_TOOLWINDOW
                       );

      SetWindowLongPtr((HWND) workspace->viewport()->winId(), GWL_EXSTYLE,
                       current_window_ex_style
                       | WS_EX_TOPMOST
                       | WS_EX_COMPOSITED
                       | WS_EX_TRANSPARENT
                       | WS_EX_ACCEPTFILES
                       );

      SetWindowPos((HWND) workspace->winId(), HWND_TOPMOST,
                   0, 0, 0, 0,
                   SWP_FRAMECHANGED
                   | SWP_NOMOVE
                   | SWP_NOSIZE
                   | SWP_NOZORDER
                   | SWP_NOOWNERZORDER);

      SetWindowPos((HWND) workspace->viewport()->winId(), HWND_TOPMOST,
                   0, 0, 0, 0,
                   SWP_FRAMECHANGED
                   | SWP_NOMOVE
                   | SWP_NOSIZE
                   | SWP_NOZORDER
                   | SWP_NOOWNERZORDER);

      PDWORD_PTR result = 0;
      ::SendMessageTimeout(ProgmanHwnd, 0x052C, 0xD, 0x1, SMTO_NORMAL, 1000, result);
      cherry_kit::system_window_context::get()->hide_native_desktop();
#endif
    }
  }

  ~Runtime() {
    std::for_each(std::begin(m_workspace_list), std::end(m_workspace_list),
                  [&](DesktopManager *a_manager) { delete a_manager; });

    m_workspace_list.clear();

    cherry_kit::resource_manager *rm = cherry_kit::resource_manager::instance();
    delete rm;
    cherry_kit::extension_manager::instance()->destroy_instance();
  }

private:
  std::vector<DesktopManager *> m_workspace_list;
};

Q_DECL_EXPORT int main(int argc, char *argv[]) {
#ifdef Q_OS_WIN32
  __reset_session_log();
  qInstallMessageHandler(__sync_session_log);
#endif

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
  cherry_kit::extension_manager *loader = 0;

  loader = cherry_kit::extension_manager::instance(
      QDir::toNativeSeparators(cherry_kit::config::instance()->prefix() +
                               QLatin1String("/share/plexy/ext/groups/")),
      QDir::toNativeSeparators(cherry_kit::config::instance()->prefix() +
                               QLatin1String("/plexyext/")));


  Runtime runtime(runtime_platform_name);

  if (runtime_platform_name)
      free(runtime_platform_name);

  return app.exec();
}
