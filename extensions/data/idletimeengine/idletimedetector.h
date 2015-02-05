#ifndef IDLETIMEDETECTOR_H
#define IDLETIMEDETECTOR_H

#include <QObject>

#include "appleidletimedetector.h"

class IdleTimeDetector : public QObject
{
  Q_OBJECT
public:
  explicit IdleTimeDetector(QObject *parent = 0);
  virtual ~IdleTimeDetector();

  virtual uint64_t duration() const;
  virtual float sleepTime() const;

Q_SIGNALS:
  void idle();
  void awake();

private:
  AppleIdleTimeDetector *const d;
};

#endif // IDLETIMEDETECTOR_H
