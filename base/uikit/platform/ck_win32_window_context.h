#ifndef WIN32_WINDOW_CONTEXT_H
#define WIN32_WINDOW_CONTEXT_H

#include <config.h>
#include <ck_system_window_context.h>


class win32_window_context : public cherry_kit::system_window_context
{
public:
    win32_window_context();
};

#endif // WIN32_WINDOW_CONTEXT_H
