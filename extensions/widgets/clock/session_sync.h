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

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void mark();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();

  void update_session();
private:
  void delete_session_data(const QString &a_session_name);
  void save_session_attribute(const QString &a_session_name,
                              const QString &a_key,
                              const QString &a_value);

  int m_session_id;
  QVariantMap m_session_data;
  QString m_session_database;
  bool m_purged;

  UIKit::ViewController *m_controller;
  UIKit::Window *m_clock_session_window;
  UIKit::Widget *m_content_view;
  UIKit::ClockWidget *m_clock_widget;
  UIKit::ToolBar *m_toolbar;
  UIKit::Label *m_timezone_label;
};

#endif // SESSIONSYNC_H
