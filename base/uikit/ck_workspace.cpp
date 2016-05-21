#include "ck_workspace.h"
#include "ck_resource_manager.h"

#include <config.h>

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QGLWidget>
#include <QImage>
#include <QPainter>
#include <QScroller>

#include <ck_data_sync.h>
#include <ck_disk_engine.h>
#include <ck_icon_button.h>
#include <ck_screen.h>
#include <ck_sync_object.h>

namespace cherry_kit {
class workspace::PrivateWorkSpace {
public:
  PrivateWorkSpace() {}
  ~PrivateWorkSpace() {}

  SpacesList m_desktop_space_list;
  QRectF m_workspace_geometry;
  QRectF m_render_box;
  int m_space_count;
  int m_current_activty_space_id;
  float m_workspace_left_margine;
  std::vector<cherry_kit::string> m_default_controller_name_list;
  bool m_opengl_on;
  int m_screen_id;
};

void workspace::set_workspace_geometry(int a_screen_id) {
  QRect _current_desktop_geometry =
      QRect(0, 0, screen::get()->x_resolution(a_screen_id),
            screen::get()->y_resolution(a_screen_id));
  setGeometry(_current_desktop_geometry);
}

workspace::workspace(QGraphicsScene *a_graphics_scene_ptr,
                     QWidget *a_parent_ptr)
    : QGraphicsView(a_graphics_scene_ptr, a_parent_ptr),
      priv(new PrivateWorkSpace) {
  priv->m_opengl_on = false;
  priv->m_screen_id = -1;

  setAttribute(Qt::WA_AcceptTouchEvents);
  setAttribute(Qt::WA_TranslucentBackground);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

#ifndef Q_OS_WIN32
  setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnBottomHint | Qt::NoDropShadowWindowHint);
#endif

#ifdef Q_OS_WIN32
  /* do not change this unless it's a must */
  setWindowFlags(Qt::CustomizeWindowHint);
  setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
#endif

  setInteractive(true);
  setFrameStyle(QFrame::NoFrame);

  setAlignment(Qt::AlignLeft | Qt::AlignTop);

  priv->m_space_count = 0;
  priv->m_current_activty_space_id = 0;
  priv->m_workspace_left_margine = 32;

  setAcceptDrops(true);
}

workspace::~workspace() { delete priv; }

void workspace::move_to_screen(int a_screen_id) {
  priv->m_screen_id = a_screen_id;
  set_workspace_geometry(a_screen_id);

  priv->m_render_box = QRectF(0, 0, get_base_width(), get_base_height());

  float height_factor = desktop_verticle_scale_factor();
  float width_factor = desktop_horizontal_scale_factor();

#if 1
  qDebug() << Q_FUNC_INFO << "geometry " << geometry();
  qDebug() << Q_FUNC_INFO << " base width " << get_base_width();
  qDebug() << Q_FUNC_INFO << "base height " << get_base_height();
  qDebug() << Q_FUNC_INFO << "scale factor width " << width_factor;
  qDebug() << Q_FUNC_INFO << "scale factor height " << height_factor;
  qDebug() << Q_FUNC_INFO << "Final W" << (width_factor * get_base_width());
  qDebug() << Q_FUNC_INFO << "Final H" << (height_factor * get_base_height());
#endif

  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                 QPainter::HighQualityAntialiasing |
                 QPainter::TextAntialiasing);

  scale(width_factor, height_factor);
}

void workspace::add_default_controller(const cherry_kit::string &a_controller_name) {
  priv->m_default_controller_name_list.push_back(a_controller_name);
}

uint workspace::space_count() const {
  return priv->m_desktop_space_list.count();
}

space *workspace::current_active_space() const {
  if (priv->m_desktop_space_list.count() <= 0) {
    return 0;
  }

  space *_current_space =
      priv->m_desktop_space_list.at(priv->m_current_activty_space_id);

  if (!_current_space) {
    qWarning() << Q_FUNC_INFO << "Error: Invalid Space Found.";
    return 0;
  }

  return _current_space;
}

void workspace::set_accelerated_rendering(bool a_on) {
  priv->m_opengl_on = a_on;

  if (priv->m_opengl_on) {
    setViewport(new QGLWidget(
        QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::SampleBuffers)));
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  } else {
    setupViewport(new QWidget);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setOptimizationFlag(QGraphicsView::DontClipPainter);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  }
}

bool workspace::is_accelerated_rendering_on() const {
  return priv->m_opengl_on;
}

void workspace::auto_switch() {
  QScroller::grabGesture(this, QScroller::TouchGesture);

  if (QScroller::hasScroller(this)) {
    QScroller *_activeScroller = QScroller::scroller(this);
    if (_activeScroller) {
      QRectF r;
      r.setX(0);
      r.setY(0.0);
      r.setHeight(this->geometry().height());
      r.setWidth(this->geometry().width() + priv->m_workspace_left_margine);

      _activeScroller->ensureVisible(r, 0, 0);
    }
  }

  QScroller::ungrabGesture(this);
}

void workspace::expose_sub_region(const QRectF &a_region) {

  /*
  QScroller::grabGesture(this, QScroller::TouchGesture);

  if (QScroller::hasScroller(this)) {
    QScroller *_active_scroller = QScroller::scroller(this);
    if (_active_scroller) {
      _active_scroller->ensureVisible(a_region, 0, 0);
    }
  } else {
    qDebug() << Q_FUNC_INFO << "No Active Scroller Found;";
  }

  QScroller::ungrabGesture(this);
  */
  QGraphicsScene *current_scene = scene();

  if (!current_scene)
    return;

  ensureVisible(a_region, 0, 0);
  update();
}

void workspace::paintEvent(QPaintEvent *event) {
  if (!priv->m_opengl_on) {
    QPainter p;
    p.begin(viewport());
    p.save();
    p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing |
                     QPainter::HighQualityAntialiasing);
    p.setBackgroundMode(Qt::TransparentMode);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(event->rect(), Qt::transparent);
    p.restore();
    p.end();
  }

#ifdef USE_QT
  QGraphicsView::paintEvent(event);
#else
  // current_active_space()->draw();

  current_active_space()->draw();
  GraphicsSurface *surface = current_active_space()->surface();
  if (!(*surface))
    return;

  QImage device((*surface), geometry().width(), geometry().height(),
                QImage::Format_ARGB32_Premultiplied);
  QPainter p;
  p.begin(viewport());
  // p.fillRect(event->rect(), QColor("#ffffff"));
  p.drawImage(event->rect(), device);
  p.end();
#endif
}

void workspace::dragEnterEvent(QDragEnterEvent *a_event_ptr) {
  QPoint _global_drop_pos = a_event_ptr->pos();
  QPoint _scene_drop_pos = mapFromGlobal(_global_drop_pos);

  qDebug() << Q_FUNC_INFO << "Dropped At:" << _scene_drop_pos;

  a_event_ptr->acceptProposedAction();
  a_event_ptr->accept();
}

void workspace::dragMoveEvent(QDragMoveEvent *a_event_ptr) {
  a_event_ptr->accept();
}

void workspace::dropEvent(QDropEvent *a_event_ptr) {
  QPoint _global_drop_pos = a_event_ptr->pos();
  QPointF _scene_drop_pos = mapToScene(mapFromGlobal(_global_drop_pos));

  for (int i = 0; i < priv->m_desktop_space_list.count(); i++) {
    space *_space = priv->m_desktop_space_list.at(i);
    if (!_space) {
      continue;
    }

    if (_space->geometry().contains(_scene_drop_pos)) {
      _space->drop_event_handler(a_event_ptr, _scene_drop_pos);
    }
  }

  a_event_ptr->setDropAction(Qt::CopyAction);
  a_event_ptr->accept();
}

void workspace::wheelEvent(QWheelEvent *a_event_ptr) {
  // ignore scroll wheel events.
  a_event_ptr->accept();
}

void workspace::focusOutEvent(QFocusEvent *a_event_ref) {
  if (current_active_space())
    current_active_space()->reset_focus();
}

float workspace::desktop_verticle_scale_factor() {
  QRectF _current_desktop_geometry(0, 0, geometry().width(),
                                   geometry().height());

  float height_factor =
      (_current_desktop_geometry.height() / get_base_height());

  return height_factor;
}

float workspace::desktop_horizontal_scale_factor() {
  QRectF _current_desktop_geometry(0, 0, geometry().width(),
                                   geometry().height());

  float width_factor = (_current_desktop_geometry.width() / get_base_width());

  return width_factor;
}

int workspace::screen_id() const { return priv->m_screen_id; }

void workspace::revoke_space(const QString &a_name, int a_id) {
  space *_space = create_blank_space();

  _space->set_workspace(this);
  _space->set_qt_graphics_scene(scene());
  _space->set_name(a_name);
  _space->set_id(a_id);

  QRectF _space_geometry;
  _space_geometry.setX(0);
  _space_geometry.setY((priv->m_workspace_geometry.height()) -
                       priv->m_render_box.height());
  _space_geometry.setHeight(priv->m_render_box.height());
  _space_geometry.setWidth(priv->m_render_box.width() +
                           priv->m_workspace_left_margine);

  _space->setGeometry(_space_geometry);
  _space->restore_session();

  priv->m_desktop_space_list << _space;
}

space *workspace::create_blank_space() {
  space *_space = new space();

  _space->set_workspace(this);
  _space->set_qt_graphics_scene(scene());

  priv->m_space_count += 1;
  priv->m_workspace_geometry.setHeight(priv->m_render_box.height() *
                                       priv->m_space_count);
  priv->m_workspace_geometry.setWidth(priv->m_render_box.width());
  priv->m_workspace_geometry.setX(0.0);
  priv->m_workspace_geometry.setY(0.0);

  setSceneRect(priv->m_workspace_geometry);

  return _space;
}

QRectF workspace::workspace_geometry() const {
  return priv->m_workspace_geometry;
}

void workspace::expose(uint a_space_id) {
  uint _current_space_count = priv->m_desktop_space_list.count();

  if (a_space_id > _current_space_count) {
    return;
  }

	space *_current_space = current_active_space();

	if (_current_space && (_current_space->id() == a_space_id)) {
			return;
	}

  space *_space = priv->m_desktop_space_list.at(a_space_id);

  if (_space) {
		_space->reset_focus();
    QRectF _space_rect = _space->geometry();
    priv->m_current_activty_space_id = a_space_id;
    expose_sub_region(_space_rect);
  }
}

space *workspace::expose_next() {
  if (priv->m_current_activty_space_id == priv->m_desktop_space_list.count()) {
    qDebug() << Q_FUNC_INFO << "Maximum Space Count Reached";
    return 0;
  }

  QRectF _space_geometry;
  priv->m_current_activty_space_id = priv->m_current_activty_space_id + 1;

  if (priv->m_current_activty_space_id >= priv->m_desktop_space_list.count()) {
    priv->m_current_activty_space_id = priv->m_desktop_space_list.count() - 1;
    return 0;
  }

  space *_next_space =
      priv->m_desktop_space_list.at(priv->m_current_activty_space_id);

  if (!_next_space) {
    qDebug() << Q_FUNC_INFO << "Invalid Space";
    return 0;
  }

  _space_geometry = _next_space->geometry();
  _space_geometry.setX(_space_geometry.width());

  this->expose_sub_region(_space_geometry);

  return _next_space;
}

space *workspace::expose_previous() {
  QRectF _space_geometry;

  priv->m_current_activty_space_id = priv->m_current_activty_space_id - 1;

  if (priv->m_current_activty_space_id < 0) {
    priv->m_current_activty_space_id = 0;
  }

  space *_prev_space =
      priv->m_desktop_space_list.at(priv->m_current_activty_space_id);

  if (!_prev_space) {
    return 0;
  }

  _space_geometry = _prev_space->geometry();
  _space_geometry.setX(_space_geometry.width() -
                       priv->m_workspace_left_margine);

  this->expose_sub_region(_space_geometry);

  return _prev_space;
}

void workspace::update_space_geometry(space *a_space_ptr,
                                      QRectF a_deleted_geometry) {
  for (int i = 0; i < priv->m_desktop_space_list.count(); i++) {
    space *__space = priv->m_desktop_space_list.at(i);
    if (!__space) {
      continue;
    }
    if (__space == a_space_ptr) {
      priv->m_desktop_space_list.removeAt(i);
    }
  }

  foreach (space *_space, priv->m_desktop_space_list) {
    if (!_space) {
      continue;
    }
    if (a_deleted_geometry.y() < _space->geometry().y()) {
      QRectF _move_geometry = _space->geometry();

      _move_geometry.setY(_move_geometry.y() - a_deleted_geometry.height());
      _move_geometry.setHeight(priv->m_render_box.height());
      _space->setGeometry(_move_geometry);
    }
  }
}

cherry_kit::string workspace::workspace_instance_name() {
  cherry_kit::string workspace_name =
      "org.workspace.space_" + std::to_string(priv->m_screen_id);

  return workspace_name;
}

void workspace::save_space_removal_session_data(const QString &a_space_name) {
  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync(workspace_instance_name());
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();
  sync->set_sync_engine(engine);

  sync->remove_object("Space", "ref", a_space_name.toStdString());

  delete sync;
}

void workspace::remove(space *a_space_ptr) {
  if (!a_space_ptr) {
    return;
  }

  QRectF _deleted_geometry = a_space_ptr->geometry();
  QString _space_ref = a_space_ptr->session_name();

  priv->m_workspace_geometry.setHeight(priv->m_workspace_geometry.height() -
                                       a_space_ptr->geometry().height());

  setSceneRect(priv->m_workspace_geometry);

  priv->m_current_activty_space_id = priv->m_current_activty_space_id - 1;

  if (priv->m_current_activty_space_id < 0) {
    priv->m_current_activty_space_id = 0;
  }

  priv->m_space_count = priv->m_space_count - 1;

  delete a_space_ptr;

  // adjust geometry of the remaining spaces.
  update_space_geometry(a_space_ptr, _deleted_geometry);

  // update the removed spaces into session
  save_space_removal_session_data(_space_ref);
}

QPixmap workspace::thumbnail(space *a_space, int a_scale_factor) {
  if (!a_space) {
    return QPixmap();
  }

  QImage _thumbnail(geometry().width() / a_scale_factor,
                    geometry().height() / a_scale_factor,
                    QImage::Format_ARGB32_Premultiplied);
  QRectF _source_rect = a_space->geometry();
  QRectF _thumbnail_geometry(0.0, 0.0, _thumbnail.width(), _thumbnail.height());
  _source_rect.setX(0);
  _source_rect.setWidth(a_space->geometry().width() -
                        priv->m_workspace_left_margine);

  QPainter _desktop_preview;

  _desktop_preview.begin(&_thumbnail);

  _desktop_preview.setRenderHints(QPainter::SmoothPixmapTransform |
                                  QPainter::Antialiasing |
                                  QPainter::HighQualityAntialiasing);
  _desktop_preview.fillRect(_thumbnail_geometry, Qt::transparent);
  _desktop_preview.setBackgroundMode(Qt::TransparentMode);
  _desktop_preview.setCompositionMode(QPainter::CompositionMode_SourceOver);

  QPainterPath _frame_path;
  _desktop_preview.save();
  _frame_path.addRoundedRect(
      QRectF(0, 0, _thumbnail.width(), _thumbnail.height()), 16.0, 16.0);

  _desktop_preview.setClipPath(_frame_path);

  _desktop_preview.fillRect(
      QRectF(0.0, 0.0, _thumbnail.width(), _thumbnail.height()),
      Qt::transparent);

  scene()->render(&_desktop_preview,
                  QRectF(0.0, 0.0, _thumbnail.width(), _thumbnail.height()),
                  _source_rect);

  _desktop_preview.restore();

  QPen pen(QColor("#F0F0F0"), 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

  _desktop_preview.setPen(pen);
  _desktop_preview.drawRoundRect(_thumbnail_geometry, 6, 6);

  _desktop_preview.end();

  return QPixmap::fromImage(_thumbnail);
}

SpacesList workspace::current_spaces() { return priv->m_desktop_space_list; }

void workspace::add_default_space() {
  space *_space = create_blank_space();

  _space->set_name("default");
  _space->set_id(priv->m_desktop_space_list.count());

  std::for_each(std::begin(priv->m_default_controller_name_list),
                std::end(priv->m_default_controller_name_list),
                [=](cherry_kit::string &a_controller_name) {
                  _space->add_controller(
                      QString::fromStdString(a_controller_name));
                });

  priv->m_desktop_space_list << _space;

  QRectF _space_geometry;
  _space_geometry.setX(0);
  _space_geometry.setY((priv->m_workspace_geometry.height()) -
                       priv->m_render_box.height());
  _space_geometry.setHeight(priv->m_render_box.height());
  _space_geometry.setWidth(priv->m_render_box.width() +
                           priv->m_workspace_left_margine);

  _space->setGeometry(_space_geometry);

  this->expose_sub_region(_space_geometry);
  priv->m_current_activty_space_id = _space->id();

  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync(workspace_instance_name());
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();
  sync->set_sync_engine(engine);

  cherry_kit::sync_object obj;
  obj.set_name("Space");
  obj.set_property("ref", _space->session_name().toStdString());
  obj.set_property("name", _space->name().toStdString());
  obj.set_key(_space->id());

  sync->add_object(obj);

  delete sync;
}

void workspace::restore_session() {
  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync(workspace_instance_name());
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {
    if (a_found) {
      QString _space_name = a_object.property("name").c_str();
      revoke_space(_space_name, a_object.key());
    }
  });

  sync->find("Space", "", "");

  delete sync;
}

float workspace::get_base_width() {
  return screen::get()->desktop_width(priv->m_screen_id);
}

float workspace::get_base_height() {
  return screen::get()->desktop_height(priv->m_screen_id);
}
}
