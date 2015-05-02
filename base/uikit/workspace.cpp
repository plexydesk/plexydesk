#include "themepackloader.h"
#include "workspace.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QScroller>
#include <QImage>
#include <QPainter>
#include <QGLWidget>
#include <datasync.h>

#include <disksyncengine.h>
#include <imagebutton.h>
#include <syncobject.h>

namespace UIKit
{
class WorkSpace::PrivateWorkSpace
{
public:
  PrivateWorkSpace() {}
  ~PrivateWorkSpace() {}

  SpacesList m_desktop_space_list;
  QRectF m_workspace_geometry;
  int m_space_count;
  int m_current_activty_space_id;
  float m_workspace_left_margine;
  QStringList m_default_controller_name_list;
  bool m_opengl_on;
};

void WorkSpace::set_workspace_geometry()
{
  QRect _current_desktop_geometry = QApplication::desktop()->screenGeometry();

#ifdef Q_OS_LINUX
  _current_desktop_geometry = QApplication::desktop()->availableGeometry();
#endif

#ifdef Q_OS_MAC
  _current_desktop_geometry.setY(
    QApplication::desktop()->availableGeometry().topLeft().y());
#endif

#ifdef Q_OS_WIN
  _current_desktop_geometry = QApplication::desktop()->availableGeometry();
#endif

  setGeometry(_current_desktop_geometry);
}

WorkSpace::WorkSpace(QGraphicsScene *a_graphics_scene_ptr, QWidget *a_parent_ptr)
  : QGraphicsView(a_graphics_scene_ptr, a_parent_ptr), m_priv_impl(new PrivateWorkSpace)
{
  m_priv_impl->m_opengl_on = false;
  setAttribute(Qt::WA_AcceptTouchEvents);
  setAttribute(Qt::WA_TranslucentBackground);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

#ifndef Q_OS_WIN
  setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnBottomHint);
#endif

  setInteractive(true);
  setFrameStyle(QFrame::NoFrame);

  setAlignment(Qt::AlignLeft | Qt::AlignTop);

  m_priv_impl->m_space_count = 0;
  m_priv_impl->m_current_activty_space_id = 0;
  m_priv_impl->m_workspace_left_margine = 0;

  set_workspace_geometry();
  setAcceptDrops(true);
}

WorkSpace::~WorkSpace() { delete m_priv_impl; }

void WorkSpace::add_default_controller(const QString &a_controller_name)
{
  m_priv_impl->m_default_controller_name_list << a_controller_name;
}

uint WorkSpace::space_count() const { return m_priv_impl->m_desktop_space_list.count(); }

Space *WorkSpace::current_active_space() const
{
  if (m_priv_impl->m_desktop_space_list.count() <= 0) {
    return 0;
  }

  Space *_current_space =
    m_priv_impl->m_desktop_space_list.at(m_priv_impl->m_current_activty_space_id);

  if (!_current_space) {
    qWarning() << Q_FUNC_INFO << "Error: Invalid Space Found.";
    return 0;
  }

  return _current_space;
}

void WorkSpace::set_accelerated_rendering(bool a_on)
{
  m_priv_impl->m_opengl_on = a_on;

  if (m_priv_impl->m_opengl_on) {
    setViewport(new QGLWidget(
                  QGLFormat(QGL::SampleBuffers |
                            QGL::DoubleBuffer |
                            QGL::DepthBuffer |
                            QGL::Rgba |
                            QGL::StencilBuffer |
                            QGL::AlphaChannel)));
    setCacheMode(QGraphicsView::CacheNone);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setOptimizationFlag(QGraphicsView::DontClipPainter);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  } else {
    setupViewport(new QWidget);
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  }
}

bool WorkSpace::is_accelerated_rendering_on() const { return m_priv_impl->m_opengl_on; }

void WorkSpace::auto_switch()
{
  QScroller::grabGesture(this, QScroller::TouchGesture);

  if (QScroller::hasScroller(this)) {
    QScroller *_activeScroller = QScroller::scroller(this);
    if (_activeScroller) {
      QRectF r;
      r.setX(m_priv_impl->m_workspace_left_margine);
      r.setY(0.0);
      r.setHeight(this->geometry().height());
      r.setWidth(this->geometry().width() + m_priv_impl->m_workspace_left_margine);

      _activeScroller->ensureVisible(r, 0, 0);
    }
  }

  QScroller::ungrabGesture(this);
}

void WorkSpace::expose_sub_region(const QRectF &a_region)
{
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
}

void WorkSpace::paintEvent(QPaintEvent *event)
{
  if (!m_priv_impl->m_opengl_on) {
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
  QGraphicsView::paintEvent(event);
}

void WorkSpace::dragEnterEvent(QDragEnterEvent *a_event_ptr)
{
  QPoint _global_drop_pos = a_event_ptr->pos();
  QPoint _scene_drop_pos = mapFromGlobal(_global_drop_pos);

  qDebug() << Q_FUNC_INFO << "Dropped At:" << _scene_drop_pos;

  a_event_ptr->acceptProposedAction();
  a_event_ptr->accept();
}

void WorkSpace::dragMoveEvent(QDragMoveEvent *a_event_ptr) { a_event_ptr->accept(); }

void WorkSpace::dropEvent(QDropEvent *a_event_ptr)
{
  QPoint _global_drop_pos = a_event_ptr->pos();
  QPointF _scene_drop_pos = mapToScene(mapFromGlobal(_global_drop_pos));

  for (int i = 0; i < m_priv_impl->m_desktop_space_list.count(); i++) {
    Space *_space = m_priv_impl->m_desktop_space_list.at(i);
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

void WorkSpace::wheelEvent(QWheelEvent *a_event_ptr)
{
  // ignore scroll wheel events.
  a_event_ptr->accept();
}

void WorkSpace::revoke_space(const QString &a_name, int a_id)
{
  Space *_space = create_blank_space();

  _space->set_workspace(this);
  _space->set_qt_graphics_scene(scene());
  _space->set_name(a_name);
  _space->set_id(a_id);
  _space->restore_session();

  QRectF _space_geometry;
  _space_geometry.setX(m_priv_impl->m_workspace_left_margine);
  _space_geometry.setY((m_priv_impl->m_workspace_geometry.height()) -
                       geometry().height());
  _space_geometry.setHeight(this->geometry().height());
  _space_geometry.setWidth(geometry().width() + m_priv_impl->m_workspace_left_margine);

  _space->setGeometry(_space_geometry);

  m_priv_impl->m_desktop_space_list << _space;
}

Space *WorkSpace::create_blank_space()
{
  Space *_space = new Space(this);

  _space->set_workspace(this);
  _space->set_qt_graphics_scene(scene());

  m_priv_impl->m_space_count += 1;

  m_priv_impl->m_workspace_geometry.setHeight(geometry().height() * m_priv_impl->m_space_count);
  m_priv_impl->m_workspace_geometry.setWidth(geometry().width());
  m_priv_impl->m_workspace_geometry.setX(0.0);
  m_priv_impl->m_workspace_geometry.setY(0.0);

  setSceneRect(m_priv_impl->m_workspace_geometry);

  return _space;
}

QRectF WorkSpace::workspace_geometry() const { return m_priv_impl->m_workspace_geometry; }

void WorkSpace::expose(uint a_space_id)
{
  uint _current_space_count = m_priv_impl->m_desktop_space_list.count();

  if (a_space_id > _current_space_count) {
    return;
  }

  Space *_space = m_priv_impl->m_desktop_space_list.at(a_space_id);

  if (_space) {
    QRectF _space_rect = _space->geometry();
    m_priv_impl->m_current_activty_space_id = a_space_id;
    expose_sub_region(_space_rect);
  }
}

Space *WorkSpace::expose_next()
{
  if (m_priv_impl->m_current_activty_space_id == m_priv_impl->m_desktop_space_list.count()) {
    qDebug() << Q_FUNC_INFO << "Maximum Space Count Reached";
    return 0;
  }

  QRectF _space_geometry;
  m_priv_impl->m_current_activty_space_id = m_priv_impl->m_current_activty_space_id + 1;

  if (m_priv_impl->m_current_activty_space_id >= m_priv_impl->m_desktop_space_list.count()) {
    m_priv_impl->m_current_activty_space_id = m_priv_impl->m_desktop_space_list.count() - 1;
    return 0;
  }

  Space *_next_space =
    m_priv_impl->m_desktop_space_list.at(m_priv_impl->m_current_activty_space_id);

  if (!_next_space) {
    qDebug() << Q_FUNC_INFO << "Invalid Space";
    return 0;
  }

  _space_geometry = _next_space->geometry();
  _space_geometry.setX(_space_geometry.width());

  this->expose_sub_region(_space_geometry);

  return _next_space;
}

Space *WorkSpace::expose_previous()
{
  QRectF _space_geometry;

  m_priv_impl->m_current_activty_space_id = m_priv_impl->m_current_activty_space_id - 1;

  if (m_priv_impl->m_current_activty_space_id < 0) {
    m_priv_impl->m_current_activty_space_id = 0;
  }

  Space *_prev_space =
    m_priv_impl->m_desktop_space_list.at(m_priv_impl->m_current_activty_space_id);

  if (!_prev_space) {
    return 0;
  }

  _space_geometry = _prev_space->geometry();
  _space_geometry.setX(_space_geometry.width() - m_priv_impl->m_workspace_left_margine);
  this->expose_sub_region(_space_geometry);

  return _prev_space;
}

void WorkSpace::update_space_geometry(Space *a_space_ptr,
                                      QRectF a_deleted_geometry)
{
  for (int i = 0; i < m_priv_impl->m_desktop_space_list.count(); i++) {
    Space *__space = m_priv_impl->m_desktop_space_list.at(i);
    if (!__space) {
      continue;
    }
    if (__space == a_space_ptr) {
      m_priv_impl->m_desktop_space_list.removeAt(i);
    }
  }

  foreach(Space * _space, m_priv_impl->m_desktop_space_list) {
    if (!_space) {
      continue;
    }
    if (a_deleted_geometry.y() < _space->geometry().y()) {
      QRectF _move_geometry = _space->geometry();
      _move_geometry.setY(_move_geometry.y() - a_deleted_geometry.height());
      _move_geometry.setHeight(this->geometry().height());
      _space->setGeometry(_move_geometry);
    }
  }
}

void WorkSpace::save_space_removal_session_data(const QString &a_space_name)
{
  QuetzalKit::DataStore *_data_store =
    new QuetzalKit::DataStore(QString("DesktopWorkSpace"), this);
  QuetzalKit::DiskSyncEngine *_engine =
    new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr = _data_store->begin("SpaceList");

  if (_session_list_ptr) {
    Q_FOREACH(QuetzalKit::SyncObject * _object,
              _session_list_ptr->childObjects()) {
      if (!_object) {
        continue;
      }
      QString _space_name = _object->attributeValue("ref").toString();

      if (_space_name == a_space_name) {
        _data_store->deleteObject(_object);
      }
    }
  }

  delete _data_store;

  //new API
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Workspace");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  delete sync;
}

void WorkSpace::remove(Space *a_space_ptr)
{
  if (!a_space_ptr) {
    return;
  }

  QRectF _deleted_geometry = a_space_ptr->geometry();
  QString _space_ref = a_space_ptr->session_name();

  m_priv_impl->m_workspace_geometry.setHeight(
              m_priv_impl->m_workspace_geometry.height() -
      a_space_ptr->geometry().height());

  setSceneRect(m_priv_impl->m_workspace_geometry);

  m_priv_impl->m_current_activty_space_id =
          m_priv_impl->m_current_activty_space_id - 1;

  if (m_priv_impl->m_current_activty_space_id < 0) {
    m_priv_impl->m_current_activty_space_id = 0;
  }

  m_priv_impl->m_space_count = m_priv_impl->m_space_count - 1;

  delete a_space_ptr;

  // adjust geometry of the remaining spaces.
  update_space_geometry(a_space_ptr, _deleted_geometry);

  // update the removed spaces into session
  save_space_removal_session_data(_space_ref);
}

QPixmap WorkSpace::thumbnail(Space *a_space, int a_scale_factor)
{
  if (!a_space) {
    return QPixmap();
  }

  QImage _thumbnail(geometry().width() / a_scale_factor,
                    geometry().height() / a_scale_factor,
                    QImage::Format_ARGB32_Premultiplied);
  QRectF _source_rect = a_space->geometry();
  QRectF _thumbnail_geometry(0.0, 0.0, _thumbnail.width(), _thumbnail.height());
  _source_rect.setX(m_priv_impl->m_workspace_left_margine);
  _source_rect.setWidth(a_space->geometry().width() -
                        m_priv_impl->m_workspace_left_margine);

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
    QRectF(0.0, 0.0, _thumbnail.width(), _thumbnail.height()), Qt::transparent);

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

SpacesList WorkSpace::current_spaces() { return m_priv_impl->m_desktop_space_list; }

void WorkSpace::add_default_space()
{
  Space *_space = create_blank_space();

  _space->set_name("default");
  _space->set_id(m_priv_impl->m_desktop_space_list.count());

  Q_FOREACH(const QString & controllerName, m_priv_impl->m_default_controller_name_list) {
    _space->add_controller(controllerName);
  }

  m_priv_impl->m_desktop_space_list << _space;

  QRectF _space_geometry;
  _space_geometry.setX(m_priv_impl->m_workspace_left_margine);
  _space_geometry.setY((m_priv_impl->m_workspace_geometry.height()) -
                       geometry().height());
  _space_geometry.setHeight(geometry().height());
  _space_geometry.setWidth(geometry().width() + m_priv_impl->m_workspace_left_margine);

  _space->setGeometry(_space_geometry);

  this->expose_sub_region(_space_geometry);
  m_priv_impl->m_current_activty_space_id = _space->id();


  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Workspace");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  QuetzalKit::SyncObject obj;
  obj.setName("Space");
  obj.setObjectAttribute("ref", _space->session_name());
  obj.setObjectAttribute("name", _space->name());
  obj.setKey(_space->id());

  sync->add_object(obj);

  delete sync;
}

void WorkSpace::restore_session()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Workspace");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                        const std::string &a_app_name, bool a_found){
      if (a_found) {
          qDebug() << Q_FUNC_INFO << "Adding Space ";
          QString _space_name = a_object.attributeValue("name").toString();
          revoke_space(_space_name, a_object.key());
      }
  });

  sync->find("Space","", "");

  delete sync;
}


}
