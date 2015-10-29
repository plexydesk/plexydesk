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
#include <thread>

// plexydesk

#include "mock_desk_runtime.h"

#include "desktopmanager.h"
#include <ck_timer.h>

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

static void start_test_runtime() {}

cherry_kit::ui_action get_action() {
  cherry_kit::ui_action action;
  action.set_name("test_task");

  return action;
}

cherry_kit::ui_action load_action() { return get_action(); }

void init_plugin_loader() {
  cherry_kit::extension_manager *loader = 0;

  loader = cherry_kit::extension_manager::instance(
      QDir::toNativeSeparators(cherry_kit::config::instance()->prefix() +
                               QLatin1String("/share/plexy/ext/groups/")),
      QDir::toNativeSeparators(cherry_kit::config::instance()->prefix() +
                               QLatin1String("/lib/plexyext/")));
}

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  //init_plugin_loader();

  /*
  cherry_kit::timer timer(1000);
  timer.on_timeout([&]() {
      qDebug() << Q_FUNC_INFO << "Timeout";
  });
  timer.start();
  */

  Runtime *runtime = new Runtime;
  runtime->open_window();

  /*
  cherry_kit::timer::start_once(1000, [=]() {
   runtime->moveToThread(QApplication::instance()->thread());
   runtime->init();
  });

  cherry_kit::timer::start_once(2000, [=]() {
      runtime->run_remove_space_test();
  });

  cherry_kit::timer::start_once(5000, [=]() {
      runtime->end();
      delete runtime;
  });
  */

  return app.exec();
}
