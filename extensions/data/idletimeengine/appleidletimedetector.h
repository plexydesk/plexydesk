#ifndef APPLEIDLETIMEDETECTOR_H
#define APPLEIDLETIMEDETECTOR_H

#include <QDateTime>
#include <QObject>

class AppleIdleTimeDetector : public QObject
{
  Q_OBJECT

public:
  explicit AppleIdleTimeDetector(QObject *parent = 0);
  virtual ~AppleIdleTimeDetector();

  void init();

  virtual quint64 idleTime() const;

  virtual void setHardwareSleepDuration(float time);

  virtual float hardwareSleepDuraiton();

  void setStartTime(const QDateTime &time);

  QDateTime startTime();

Q_SIGNALS:
  void hardwareAwake();

private:
  class AppleIdleTimeDetectorPrivate;
  AppleIdleTimeDetectorPrivate *const d;
};

#endif // APPLEIDLETIMEDETECTOR_H
