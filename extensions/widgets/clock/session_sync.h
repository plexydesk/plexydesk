#ifndef SESSIONSYNC_H
#define SESSIONSYNC_H

#include <QVariant>
#include <view_controller.h>
#include <toolbar.h>
#include <label.h>
#include <clockwidget.h>

class SessionSync {
public:
  SessionSync(UIKit::ViewController *a_controller,
                 const QVariantMap &a_data = QVariantMap());
  ~SessionSync() {}

  virtual void session_init();

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void mark();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();

  virtual void update_session();

  virtual void delete_session_data(const QString &a_session_name);
  virtual void save_session_attribute(const QString &a_session_name,
                              const QString &a_key,
                              const QString &a_value);
private:
  class PrivSessionSync;
  PrivSessionSync *const d;
};

#endif // SESSIONSYNC_H
