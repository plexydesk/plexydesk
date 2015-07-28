#ifndef MOCK_DESK_RUNTIME_H
#define MOCK_DESK_RUNTIME_H

// plexydesk

#include <ck_workspace.h>
#include <ck_config.h>
#include <ck_extension_manager.h>

#if defined(Q_OS_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>
#include <X11/Xutil.h>

#include <netwm.h>
#endif

#include "desktopmanager.h"


class Runtime : public QObject {
    Q_OBJECT
public:
  explicit Runtime(QObject *parent = 0);

  ~Runtime();

public Q_SLOTS:
  void init();
  void run_remove_space_test();
  void end();

private:
  DesktopManager *m_manager;
};

#endif // MOCK_DESK_RUNTIME_H
