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
#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QGraphicsView>

#include <ck_space.h>
#include <plexydesk_ui_exports.h>

#ifdef USE_QT
typedef QGraphicsView PlatformScrollView;
#endif

namespace cherry_kit {
typedef QList<space *> SpacesList;

class DECL_UI_KIT_EXPORT workspace : public QGraphicsView {
  Q_OBJECT
public:
  workspace(QGraphicsScene *a_graphics_scene_ptr, QWidget *a_parent_ptr = 0);
  virtual ~workspace();

  virtual void move_to_screen(int a_screen_id);

  virtual void add_default_controller(const std::string &a_controller_name);

  virtual space *create_blank_space();
  virtual void add_default_space();

  virtual void remove(space *a_space_ptr);
  virtual void auto_switch();
  virtual void expose(uint a_space_id);
  virtual space *expose_next();
  virtual space *expose_previous();
  virtual void expose_sub_region(const QRectF &a_region);

  virtual SpacesList current_spaces();
  virtual space *current_active_space() const;
  virtual uint space_count() const;

  virtual void restore_session();
  void revoke_space(const QString &a_name, int a_id);

  virtual QRectF workspace_geometry() const;

  virtual QPixmap thumbnail(space *a_space, int a_scale_factor = 10);

  virtual void set_accelerated_rendering(bool a_on = true);
  virtual bool is_accelerated_rendering_on() const;

  std::string workspace_instance_name();
  float desktop_verticle_scale_factor();
  float desktop_horizontal_scale_factor();

protected:
  virtual void paintEvent(QPaintEvent *a_event_ptr);
  virtual void dragEnterEvent(QDragEnterEvent *a_event_ptr);
  virtual void dragMoveEvent(QDragMoveEvent *a_event_ptr);
  virtual void dropEvent(QDropEvent *a_event_ptr);
  virtual void wheelEvent(QWheelEvent *a_event_ptr);
  virtual void focusOutEvent(QFocusEvent *a_event_ref);

private:
  class PrivateWorkSpace;
  PrivateWorkSpace *const priv;

  void update_space_geometry(space *a_space_ptr, QRectF a_deleted_geometry);
  void set_workspace_geometry(int a_screen_id);

  void save_space_removal_session_data(const QString &a_space_name);

  float get_base_height();
  float get_base_width();
};
}
#endif // WORKSPACE_H
