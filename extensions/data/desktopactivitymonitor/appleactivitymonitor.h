#ifndef APPLEACTIVITYMONITOR_H
#define APPLEACTIVITYMONITOR_H

#include <QObject>

class AppleActivityMonitor : public QObject {
  Q_OBJECT
public:
  explicit AppleActivityMonitor(QObject *parent = 0);

signals:

public
slots:
};

#endif // APPLEACTIVITYMONITOR_H
