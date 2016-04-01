#include "ck_win32_window_context.h"

#include <Windows.h>
#include <tchar.h>
#include <clrdata.h>

namespace cherry_kit {
win32_window_context::win32_window_context()
{
}

win32_window_context::~win32_window_context()
{
}

device_window *win32_window_context::find_window(const std::string &a_title) {
  device_window *rv = 0;
  return rv;
}

bool win32_window_context::hide_native_desktop()
{
    bool rv = 0;

    HWND target_handle = NULL;

    if (get_windows_major_version() < 6) {
        target_handle = FindWindow(_T("Progman"), _T("Program Manager"));
    } else {
        HWND hShellWnd = GetShellWindow();
        //target_handle =
        //        FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
        target_handle = FindWindow(_T("Progman"), _T("Program Manager"));

        if (target_handle == NULL) {
            return rv;
        }
    }

    SendMessage(target_handle, 0x0111, 0x7402, 0);

    rv = 1;
    return rv;
}

int win32_window_context::get_windows_major_version()
{
    OSVERSIONINFO os_version_info;
    ZeroMemory(&os_version_info, sizeof(OSVERSIONINFO));
    os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionExA(&os_version_info);
    return os_version_info.dwMajorVersion;
}

}
