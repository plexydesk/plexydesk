#ifndef PD_SPACE_H
#define PD_SPACE_H

#include <QGraphicsItem>
#include <QObject>

#include <view_controller.h>
#include <plexydesk_ui_exports.h>
#include <widget.h>

namespace UIKit
{
class WorkSpace;

class DECL_UI_KIT_EXPORT Space : public QObject
{
  Q_OBJECT
public:
  typedef enum {
    kCenterOnViewport,
    kCenterOnViewportLeft,
    kCenterOnViewportRight,
    kCenterOnViewportTop,
    kCenterOnViewportBottom
  } ViewportLocation;

  typedef enum {
    kControllerAddedNotification,
    kControllerRemovedNotification,
    kGeometryChangedNotification,
    kOrientationChangedNotification,
    kSpaceSwitchNotification,
    kObjectDroppedNotification
  } ViewportNotificationType;

  explicit Space(QObject *a_parent = 0);
  virtual ~Space();

  virtual void add_controller(const QString &a_name);
  virtual ViewControllerPtr controller(const QString &a_name);
  virtual QStringList current_controller_list() const;

  virtual void set_name(const QString &a_name);
  virtual QString name() const;

  virtual void set_id(int a_id);
  virtual int id() const;

  virtual QObject *workspace();
  virtual void set_workspace(WorkSpace *a_workspace_ptr);

  virtual void restore_session();

  virtual void set_qt_graphics_scene(QGraphicsScene *a_qt_graphics_scene_ptr);

  virtual void setGeometry(const QRectF &a_geometry);
  virtual QRectF geometry() const;

  QString session_name() const;
  virtual void update_session_value(const QString &a_name,
                                    const QString &a_key,
                                    const QString &a_value);

  virtual void add_activity(UIKit::DesktopActivityPtr a_activity_ptr);
  UIKit::DesktopActivityPtr create_activity(const QString &a_activity,
      const QString &a_title,
      const QPointF &a_pos,
      const QRectF &a_rect,
      const QVariantMap &a_data_map);

  virtual QPointF cursor_pos() const;
  virtual QPointF center(const QRectF &a_view_geometry,
                         const ViewportLocation &a_location = kCenterOnViewport
                        ) const;

  virtual void drop_event_handler(QDropEvent *event, const QPointF &event_pos);

  virtual void insert_window_to_view(Window *a_window);
  virtual void remove_window_from_view(Window *a_window);

  virtual void on_viewport_event_notify(
    std::function<void (ViewportNotificationType,
                        const QVariant &,
                        const Space *)> a_notify_handler);
public Q_SLOTS:
  virtual void on_activity_finished();
protected:
  virtual void clear();
  void register_controller(const QString &a_controller_name);
private:
  class PrivateSpace;
  PrivateSpace *const m_priv_impl;
};
}
#endif // SPACE_H
