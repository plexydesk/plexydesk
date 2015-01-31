#include "themepackloader.h"
#include "workspace.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QScroller>
#include <QImage>
#include <QPainter>
#include <QGLWidget>

#include <disksyncengine.h>
#include <imagebutton.h>
#include <syncobject.h>

namespace PlexyDesk {
class WorkSpace::PrivateWorkSpace {
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

void WorkSpace::setWorkspaceGeometry() {
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

WorkSpace::WorkSpace(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), d(new PrivateWorkSpace) {
  d->m_opengl_on = false;
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

  d->m_space_count = 0;
  d->m_current_activty_space_id = 0;
  d->m_workspace_left_margine = 0;

  setWorkspaceGeometry();
  setAcceptDrops(true);
}

WorkSpace::~WorkSpace() { delete d; }

void WorkSpace::addDefaultController(const QString &name) {
  d->m_default_controller_name_list << name;
}

void WorkSpace::restoreSession() {
  QuetzalKit::DataStore *_data_store =
      new QuetzalKit::DataStore(QString("DesktopWorkSpace"), this);
  QuetzalKit::DiskSyncEngine *_engine =
      new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr = _data_store->begin("SpaceList");

  if (_session_list_ptr) {
    Q_FOREACH(const QuetzalKit::SyncObject * _object,
              _session_list_ptr->childObjects()) {
      if (!_object)
        continue;
      QString _space_name = _object->attributeValue("name").toString();

      qDebug() << Q_FUNC_INFO << "Adding Space : " << _space_name;
      revokeSpace(_space_name, _object->key());
    }
  }

  delete _data_store;
}

uint WorkSpace::spaceCount() const { return d->m_desktop_space_list.count(); }

Space *WorkSpace::currentVisibleSpace() const {
  if (d->m_desktop_space_list.count() <= 0)
    return 0;

  Space *_current_space =
      d->m_desktop_space_list.at(d->m_current_activty_space_id);

  if (!_current_space) {
    qWarning() << Q_FUNC_INFO << "Error: Invalid Space Found.";
    return 0;
  }

  return _current_space;
}

void WorkSpace::setAcceleratedRendering(bool on) {
  d->m_opengl_on = on;

  if (d->m_opengl_on) {
    setViewport(new QGLWidget(
        QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer | QGL::DepthBuffer |
                  QGL::Rgba | QGL::StencilBuffer | QGL::AlphaChannel)));
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

bool WorkSpace::isAcceleratedRenderingOn() const { return d->m_opengl_on; }

void WorkSpace::switchSpace() {
  QScroller::grabGesture(this, QScroller::TouchGesture);

  if (QScroller::hasScroller(this)) {
    QScroller *_activeScroller = QScroller::scroller(this);
    if (_activeScroller) {
      QRectF r;
      r.setX(d->m_workspace_left_margine);
      r.setY(0.0);
      r.setHeight(this->geometry().height());
      r.setWidth(this->geometry().width() + d->m_workspace_left_margine);

      _activeScroller->ensureVisible(r, 0, 0);
    }
  }

  QScroller::ungrabGesture(this);
}

void WorkSpace::exposeSubRegion(const QRectF &rect) {
  QScroller::grabGesture(this, QScroller::TouchGesture);
  if (QScroller::hasScroller(this)) {
    QScroller *_active_scroller = QScroller::scroller(this);
    if (_active_scroller) {
      _active_scroller->ensureVisible(rect, 0, 0);
    }
  } else {
    qDebug() << Q_FUNC_INFO << "No Active Scroller Found;";
  }

  QScroller::ungrabGesture(this);
}

void WorkSpace::paintEvent(QPaintEvent *event) {
  if (!d->m_opengl_on) {
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

void WorkSpace::dragEnterEvent(QDragEnterEvent *event) {
  QPoint _global_drop_pos = event->pos();
  QPoint _scene_drop_pos = mapFromGlobal(_global_drop_pos);

  qDebug() << Q_FUNC_INFO << "Dropped At:" << _scene_drop_pos;

  event->acceptProposedAction();
  event->accept();
}

void WorkSpace::dragMoveEvent(QDragMoveEvent *event) { event->accept(); }

void WorkSpace::dropEvent(QDropEvent *event) {
  QPoint _global_drop_pos = event->pos();
  QPointF _scene_drop_pos = mapToScene(mapFromGlobal(_global_drop_pos));

  for (int i = 0; i < d->m_desktop_space_list.count(); i++) {
    Space *_space = d->m_desktop_space_list.at(i);
    if (!_space)
      continue;

    if (_space->geometry().contains(_scene_drop_pos)) {
      _space->handleDropEvent(event, _scene_drop_pos);
    }
  }

  event->setDropAction(Qt::CopyAction);
  event->accept();
}

void WorkSpace::wheelEvent(QWheelEvent *event) {
  // ignore scroll wheel events.
  event->accept();
}

void WorkSpace::revokeSpace(const QString &name, int id) {
  Space *_space = createEmptySpace();

  _space->setWorkspace(this);
  _space->setScene(this->scene());
  _space->setName(name);
  _space->setId(id);
  _space->restoreSession();

  QRectF _space_geometry;
  _space_geometry.setX(d->m_workspace_left_margine);
  _space_geometry.setY((d->m_workspace_geometry.height()) -
                       geometry().height());
  _space_geometry.setHeight(this->geometry().height());
  _space_geometry.setWidth(geometry().width() + d->m_workspace_left_margine);

  _space->setSpaceGeometry(_space_geometry);

  d->m_desktop_space_list << _space;
}

Space *WorkSpace::createEmptySpace() {
  Space *_space = new Space(this);

  _space->setWorkspace(this);
  _space->setScene(scene());

  d->m_space_count += 1;

  d->m_workspace_geometry.setHeight(geometry().height() * d->m_space_count);
  d->m_workspace_geometry.setWidth(geometry().width());
  d->m_workspace_geometry.setX(0.0);
  d->m_workspace_geometry.setY(0.0);

  setSceneRect(d->m_workspace_geometry);

  return _space;
}

QRectF WorkSpace::workspaceGeometry() const { return d->m_workspace_geometry; }

void WorkSpace::exposeSpace(uint id) {
  uint _current_space_count = d->m_desktop_space_list.count();

  if (id > _current_space_count) {
    return;
  }

  Space *_space = d->m_desktop_space_list.at(id);

  if (_space) {
    QRectF _space_rect = _space->geometry();
    d->m_current_activty_space_id = id;
    exposeSubRegion(_space_rect);
  }
}

Space *WorkSpace::exposeNextSpace() {
  if (d->m_current_activty_space_id == d->m_desktop_space_list.count()) {
    qDebug() << Q_FUNC_INFO << "Maximum Space Count Reached";
    return 0;
  }

  QRectF _space_geometry;
  d->m_current_activty_space_id = d->m_current_activty_space_id + 1;

  if (d->m_current_activty_space_id >= d->m_desktop_space_list.count()) {
    d->m_current_activty_space_id = d->m_desktop_space_list.count() - 1;
    return 0;
  }

  Space *_next_space =
      d->m_desktop_space_list.at(d->m_current_activty_space_id);

  if (!_next_space) {
    qDebug() << Q_FUNC_INFO << "Invalid Space";
    return 0;
  }

  _space_geometry = _next_space->geometry();
  _space_geometry.setX(_space_geometry.width());

  this->exposeSubRegion(_space_geometry);

  return _next_space;
}

Space *WorkSpace::exposePreviousSpace() {
  QRectF _space_geometry;

  d->m_current_activty_space_id = d->m_current_activty_space_id - 1;

  if (d->m_current_activty_space_id < 0)
    d->m_current_activty_space_id = 0;

  Space *_prev_space =
      d->m_desktop_space_list.at(d->m_current_activty_space_id);

  if (!_prev_space)
    return 0;

  _space_geometry = _prev_space->geometry();
  _space_geometry.setX(_space_geometry.width() - d->m_workspace_left_margine);
  this->exposeSubRegion(_space_geometry);

  return _prev_space;
}

void WorkSpace::updateSpaceGeometry(Space *space, QRectF _deleted_geometry) {
  for (int i = 0; i < d->m_desktop_space_list.count(); i++) {
    Space *__space = d->m_desktop_space_list.at(i);
    if (!__space)
      continue;
    if (__space == space) {
      d->m_desktop_space_list.removeAt(i);
    }
  }

  foreach(Space * _space, d->m_desktop_space_list) {
    if (!_space)
      continue;
    if (_deleted_geometry.y() < _space->geometry().y()) {
      QRectF _move_geometry = _space->geometry();
      _move_geometry.setY(_move_geometry.y() - _deleted_geometry.height());
      _move_geometry.setHeight(this->geometry().height());
      _space->setSpaceGeometry(_move_geometry);
    }
  }
}

void WorkSpace::saveSpaceRemovalSessionData(QString _space_ref) {
  QuetzalKit::DataStore *_data_store =
      new QuetzalKit::DataStore(QString("DesktopWorkSpace"), this);
  QuetzalKit::DiskSyncEngine *_engine =
      new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr = _data_store->begin("SpaceList");

  if (_session_list_ptr) {
    Q_FOREACH(QuetzalKit::SyncObject * _object,
              _session_list_ptr->childObjects()) {
      if (!_object)
        continue;
      QString _space_name = _object->attributeValue("ref").toString();

      if (_space_name == _space_ref) {
        _data_store->deleteObject(_object);
      }
    }
  }

  delete _data_store;
}

void WorkSpace::removeSpace(Space *space) {
  if (!space)
    return;

  QRectF _deleted_geometry = space->geometry();
  QString _space_ref = space->sessionName();

  d->m_workspace_geometry.setHeight(d->m_workspace_geometry.height() -
                                    space->geometry().height());

  setSceneRect(d->m_workspace_geometry);

  d->m_current_activty_space_id = d->m_current_activty_space_id - 1;

  if (d->m_current_activty_space_id < 0)
    d->m_current_activty_space_id = 0;

  d->m_space_count = d->m_space_count - 1;

  delete space;

  // adjust geometry of the remaining spaces.
  updateSpaceGeometry(space, _deleted_geometry);

  // update the removed spaces into session
  saveSpaceRemovalSessionData(_space_ref);
}

QPixmap WorkSpace::previewSpace(Space *space, int scaleFactor) {
  if (!space)
    return QPixmap();

  QImage _thumbnail(geometry().width() / scaleFactor,
                    geometry().height() / scaleFactor,
                    QImage::Format_ARGB32_Premultiplied);
  QRectF _source_rect = space->geometry();
  QRectF _thumbnail_geometry(0.0, 0.0, _thumbnail.width(), _thumbnail.height());
  _source_rect.setX(d->m_workspace_left_margine);
  _source_rect.setWidth(space->geometry().width() -
                        d->m_workspace_left_margine);

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
      QRectF(0.0, 0.0, _thumbnail.width(), _thumbnail.height()), Qt::white);

  scene()->render(&_desktop_preview,
                  QRectF(0.0, 0.0, _thumbnail.width(), _thumbnail.height()),
                  _source_rect);

  _desktop_preview.restore();

  QPen pen(QColor("#F0F0F0"), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

  _desktop_preview.setPen(pen);
  _desktop_preview.drawRect(_thumbnail_geometry);

  _desktop_preview.end();

  return QPixmap::fromImage(_thumbnail);
}

SpacesList WorkSpace::currentSpaces() { return d->m_desktop_space_list; }

void WorkSpace::addSpace() {
  Space *_space = createEmptySpace();

  _space->setName("default");
  _space->setId(d->m_desktop_space_list.count());

  Q_FOREACH(const QString & controllerName, d->m_default_controller_name_list) {
    _space->addController(controllerName);
  }

  d->m_desktop_space_list << _space;

  QRectF _space_geometry;
  _space_geometry.setX(d->m_workspace_left_margine);
  _space_geometry.setY((d->m_workspace_geometry.height()) -
                       geometry().height());
  _space_geometry.setHeight(geometry().height());
  _space_geometry.setWidth(geometry().width() + d->m_workspace_left_margine);

  _space->setSpaceGeometry(_space_geometry);

  this->exposeSubRegion(_space_geometry);
  d->m_current_activty_space_id = _space->id();

  // Add this space to Session
  QuetzalKit::DataStore *_data_store =
      new QuetzalKit::DataStore(QString("DesktopWorkSpace"), this);
  QuetzalKit::DiskSyncEngine *_engine =
      new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr = _data_store->begin("SpaceList");
  QuetzalKit::SyncObject *_new_space_ptr =
      _session_list_ptr->createNewObject("Space");

  _new_space_ptr->setObjectAttribute("ref", _space->sessionName());
  _new_space_ptr->setObjectAttribute("name", _space->name());

  _data_store->insert(_new_space_ptr);

  delete _data_store;

  Q_EMIT workspaceChange();
}
}
