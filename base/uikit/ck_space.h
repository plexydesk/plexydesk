/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#ifndef PD_SPACE_H
#define PD_SPACE_H

#include <ck_desktop_controller_interface.h>
#include <plexydesk_ui_exports.h>
#include <ck_widget.h>
#include <ck_sync_object.h>

namespace cherry_kit {
class workspace;

class DECL_UI_KIT_EXPORT space {

  friend class desktop_controller_interface;
  friend class workspace;

public:
  typedef enum {
    kCenterOnViewport,
    kCenterOnViewportLeft,
    kCenterOnViewportRight,
    kCenterOnViewportTop,
    kCenterOnViewportBottom,
    kCenterOnWindow,
  } ViewportLocation;

  typedef enum {
    kControllerAddedNotification,
    kControllerRemovedNotification,
    kGeometryChangedNotification,
    kOrientationChangedNotification,
    kSpaceSwitchNotification,
    kObjectDroppedNotification
  } ViewportNotificationType;

  explicit space();
  virtual ~space();

  virtual void add_controller(const QString &a_name);
  virtual desktop_controller_ref controller(const QString &a_name);
  virtual QStringList current_controller_list() const;

  virtual workspace *owner_workspace();
  virtual void set_workspace(workspace *a_workspace_ptr);

  virtual void setGeometry(const QRectF &a_geometry);
  virtual QRectF geometry() const;

  QString session_name() const;
  QString session_name_for_controller(const QString &a_controller_name);

  virtual void update_session_value(const QString &a_controller_name,
                                    const QString &a_key,
                                    const QString &a_value);

  cherry_kit::desktop_dialog_ref open_desktop_dialog(const QString &a_activity,
                                                 const QString &a_title,
                                                 const QPointF &a_pos,
                                                 const QRectF &a_rect,
                                                 const QVariantMap &a_data_map);

  virtual QPointF cursor_pos() const;
  virtual QPointF
  center(const QRectF &a_view_geometry,
         const QRectF a_window_geometry = QRectF(),
         const ViewportLocation &a_location = kCenterOnViewport) const;

  virtual void on_viewport_event_notify(
      std::function<void(ViewportNotificationType, const QVariant &,
                         const space *)> a_notify_handler);
  virtual void on_activity_finished(const desktop_dialog *a_activity);

  virtual int id() const;
protected:
  virtual void clear();
  void register_controller(const QString &a_controller_name);

  virtual void remove_window_from_view(window *a_window);
  virtual void insert_window_to_view(window *a_window);

  virtual void drop_event_handler(QDropEvent *event, const QPointF &event_pos);

  virtual void set_name(const QString &a_name);
  virtual QString name() const;

  virtual void set_id(int a_id);

  virtual void restore_session();

  virtual void set_qt_graphics_scene(QGraphicsScene *a_qt_graphics_scene_ptr);

  void save_controller_to_session(const QString &a_controller_name);
  void revoke_controller_session_attributes(const QString &a_controller_name);

  // experimental:
  virtual void draw();
  virtual GraphicsSurface *surface();

private:
  void add_activity(cherry_kit::desktop_dialog_ref a_activity_ptr);

  class PrivateSpace;
  PrivateSpace *const o_space;
};
}
#endif // SPACE_H
