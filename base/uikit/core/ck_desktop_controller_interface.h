#ifndef PLEXY_VIEW_CONTROLLER_PLUGIN_H
#define PLEXY_VIEW_CONTROLLER_PLUGIN_H

#define QT_SHAREDPOINTER_TRACK_POINTERS 1

#include <QSharedPointer>
#include <QAction>

#include <ck_data_source.h>
#include <ck_desktop_dialog.h>
#include <ck_space.h>
#include <plexydesk_ui_exports.h>

#include <ck_sync_object.h>
#include <ck_ui_action.h>
#include <ck_session_sync.h>

namespace cherry_kit {

class widget;
class session_sync;

typedef QList<QAction *> ActionList;

class DECL_UI_KIT_EXPORT desktop_controller_interface : public QObject {
  Q_OBJECT

  friend class space;
public:
  virtual void init() = 0;
  virtual ~desktop_controller_interface();

  virtual void set_view_rect(const QRectF &a_rect) = 0;

  void set_viewport(space *a_view_ptr);
  virtual space *viewport() const;

  virtual ActionList actions() const;
  virtual ui_action task() const {
      return ui_action();
  }

  virtual void request_action(const QString &a_actionName,
                              const QVariantMap &a_args = QVariantMap());

  virtual data_source *dataSource();

  virtual void insert(window *a_window_ptr);

  virtual void set_controller_name(const QString &a_name);
  virtual QString controller_name() const;

protected:
  explicit desktop_controller_interface(QObject *a_parent_ptr = 0);
  virtual void
  revoke_previous_session(const std::string &a_session_object_name,
                          std::function<void(desktop_controller_interface *,
                                             session_sync *)> a_callback);
  virtual void write_session_data(const std::string &a_session_name);
  virtual int session_count();

  virtual void session_data_ready(const cherry_kit::sync_object &a_root) = 0;
  virtual void submit_session_data(cherry_kit::sync_object *a_root_obj) = 0;
  virtual void start_session(const std::string &a_session_name,
                             const QVariantMap &a_data, bool a_restore,
                             std::function<void(desktop_controller_interface *,
                                                session_sync *)> a_callback);
  virtual std::string session_store_name(const std::string &a_name) const;

  virtual void handle_drop_event(widget *a_widget_ptr, QDropEvent *a_event_ptr);

  virtual void prepare_removal();
  virtual bool remove_widget(widget *a_widget_ptr);

private:
  class PrivateViewControllerPlugin;
  PrivateViewControllerPlugin *const priv;
};

typedef QSharedPointer<desktop_controller_interface> desktop_controller_ref;
}
#endif
