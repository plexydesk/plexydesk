#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QGraphicsView>

#include <space.h>
#include <plexydesk_ui_exports.h>

namespace UIKit
{
typedef QList<Space *> SpacesList;

class DECL_UI_KIT_EXPORT WorkSpace : public QGraphicsView
{
  Q_OBJECT
public:
  WorkSpace(QGraphicsScene *a_graphics_scene_ptr, QWidget *a_parent_ptr = 0);
  virtual ~WorkSpace();

  virtual void add_default_controller(const QString &a_controller_name);

  virtual Space *create_blank_space();
  virtual void add_default_space();

  virtual void remove(Space *a_space_ptr);
  virtual void auto_switch();
  virtual void expose(uint a_space_id);
  virtual Space *expose_next();
  virtual Space *expose_previous();
  virtual void expose_sub_region(const QRectF &a_region);

  virtual SpacesList current_spaces();
  virtual Space *current_active_space() const;
  virtual uint space_count() const;

  virtual void restore_session();
  void revoke_space(const QString &a_name, int a_id);

  virtual QRectF workspace_geometry() const;

  virtual QPixmap thumbnail(Space *a_space, int a_scale_factor = 10);

  virtual void set_accelerated_rendering(bool a_on = true);
  virtual bool is_accelerated_rendering_on() const;

protected:
  virtual void paintEvent(QPaintEvent *event);
  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dragMoveEvent(QDragMoveEvent *event);
  virtual void dropEvent(QDropEvent *event);
  virtual void wheelEvent(QWheelEvent *event);

private:
  class PrivateWorkSpace;
  PrivateWorkSpace *const m_priv_impl;

  void update_space_geometry(Space *a_space_ptr, QRectF a_deleted_geometry);
  void set_workspace_geometry();

  void save_space_removal_session_data(const QString &a_space_name);
};
}
#endif // WORKSPACE_H
