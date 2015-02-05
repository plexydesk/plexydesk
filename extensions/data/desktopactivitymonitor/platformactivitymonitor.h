#ifndef PLATFORMACTIVITYMONITOR_H
#define PLATFORMACTIVITYMONITOR_H

#include <QObject>

class WindowObject;

class PlatformActivityMonitor : public QObject
{
  Q_OBJECT
public:
  explicit PlatformActivityMonitor(QObject *parent = 0);
  virtual ~PlatformActivityMonitor();

  WindowObject updateWindowList();

  void next();

  void timeout();

private:
  class PlatformActivityMonitorPrivate;
  PlatformActivityMonitorPrivate *const d;
};

#endif // PLATFORMACTIVITYMONITOR_H
