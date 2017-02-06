#include "ck_win32_window_context.h"

#ifndef CK_UWP_BUILD
#include <Windows.h>
#include <tchar.h>
#include <clrdata.h>
#endif

#include <ck_device_window.h>

#include <QDebug>

typedef struct {
  std::string window_name;
  std::string window_class;
  cherry_kit::window_handle_t window_handle;
} window_lookup_data_t;

BOOL CALLBACK __match_window_hanlde(cherry_kit::window_handle_t a_id,
                                    LPARAM a_ctx) {
 window_lookup_data_t *query = (window_lookup_data_t *)a_ctx;
 char _window_class_name[100];
 char *_window_text = NULL;

 if (!query)
     return TRUE;

#ifndef CK_UWP_BUILD 
 int window_title_size = GetWindowTextLength(a_id);
 _window_text = (char *) malloc(window_title_size + 1);

 RealGetWindowClass(a_id, _window_class_name, 100);
 GetWindowText(a_id, _window_text, window_title_size + 1);

 if ((std::strcmp(query->window_name.c_str(), _window_text) == 0) &&
     (std::strcmp(query->window_class.c_str(), _window_class_name) == 0)) {
   query->window_handle = a_id;
   return FALSE;
 }
#endif
 return TRUE;
}

namespace cherry_kit {
win32_window_context::win32_window_context() {
}

win32_window_context::~win32_window_context() {
}

device_window *win32_window_context::find_window(
        const std::string &a_title, const std::string &a_window_class) {
  cherry_kit::device_window *rv = new device_window();
  cherry_kit::window_handle_t window_handle = 0;
  window_lookup_data_t data;

  data.window_class = a_window_class;
  data.window_name = a_title;

#ifndef CK_UWP_BUILD
  EnumChildWindows(GetDesktopWindow(),
                   __match_window_hanlde,
                   reinterpret_cast<LPARAM>(&data));

  rv->set_handle(data.window_handle);
#endif
  return rv;
}

device_window *win32_window_context::desktop() {
  cherry_kit::device_window *rv = new device_window();
  cherry_kit::device_window *shell_view = 0;
  cherry_kit::window_handle_t window_handle = 0;
  window_lookup_data_t data;

  data.window_name = "FolderView";
  data.window_class = "SysListView32";

#ifndef CK_UWP_BUILD
    bool rv = 0;
  shell_view = find_window("", "SHELLDLL_DefView");

  if (shell_view) {
    EnumChildWindows(shell_view->handle(),
                     __match_window_hanlde,
                     reinterpret_cast<LPARAM>(&data));

     rv->set_handle(data.window_handle);
  }
#endif
  return rv;
}

bool win32_window_context::hide_native_desktop()
{
#ifndef CK_UWP_BUILD
    bool rv = 0;

    HWND target_handle = NULL;

    if (get_windows_major_version() < 6) {
        target_handle = FindWindow(_T("Progman"), _T("Program Manager"));
    } else {
        HWND hShellWnd = GetShellWindow();
        //target_handle =
        //FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
        target_handle = FindWindow(_T("Progman"), _T("Program Manager"));

        if (target_handle == NULL) {
            return rv;
        }
    }

    SendMessage(target_handle, 0x0111, 0x7402, 0);

    rv = 1;
    return rv;
#else
	return 0;
#endif
}

int win32_window_context::get_windows_major_version()
{
#ifndef CK_UWP_BUILD
    bool rv = 0;
    OSVERSIONINFO os_version_info;
    ZeroMemory(&os_version_info, sizeof(OSVERSIONINFO));
    os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionExA(&os_version_info);
    return os_version_info.dwMajorVersion;
#else
	return 0;
#endif
}
}
