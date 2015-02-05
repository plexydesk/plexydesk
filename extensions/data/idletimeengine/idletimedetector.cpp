#include "idletimedetector.h"

IdleTimeDetector::IdleTimeDetector(QObject *parent)
  : QObject(parent), d(new AppleIdleTimeDetector)
{
  d->init();
  connect(d, SIGNAL(hardwareAwake()), this, SIGNAL(awake()));
}

IdleTimeDetector::~IdleTimeDetector() { delete d; }

uint64_t IdleTimeDetector::duration() const { return d->idleTime(); }

float IdleTimeDetector::sleepTime() const { return d->hardwareSleepDuraiton(); }
