#ifndef WINDOWOBJECT_H
#define WINDOWOBJECT_H

#include <QSharedDataPointer>
#include <QTime>

#include <syncobject.h>

class WindowObjectData;

class WindowObject {
public:
  WindowObject();
  WindowObject(const WindowObject &);
  WindowObject &operator=(const WindowObject &);
  ~WindowObject();

  QString name() const;
  void setName(const QString &name);

  QString title() const;
  void setTitle(const QString &title);

  QTime time() const;
  void setTime(const QTime &time);

  int duration() const;
  void updateDuration(int duration);

  void sync();

private:
  QSharedDataPointer<WindowObjectData> data;
};

#endif // WINDOWOBJECT_H
