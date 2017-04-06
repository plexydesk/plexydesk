#include "ck_workspace.h"
#include "ck_resource_manager.h"

#include <config.h>

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QtOpenGL/QGLWidget>
#include <QImage>
#include <QPainter>

#ifdef __QT5_TOOLKIT__
#include <QScroller>
#endif

#include <ck_data_sync.h>
#include <ck_disk_engine.h>
#include <ck_icon_button.h>
#include <ck_image_view.h>
#include <ck_item_view.h>
#include <ck_screen.h>
#include <ck_sync_object.h>

#include <vector>
#include <algorithm>

namespace cherry_kit {
/* internal desktop compositor to manage spaces context */
class wp_compositor {
public:
    typedef std::vector<window *> cmp_window_list_t;
    typedef std::vector<widget *> cmp_widget_list_t;
    typedef std::vector<const space *> cmp_space_list_t;

    wp_compositor() {}
    virtual ~wp_compositor() {
        if (m_cmp_window)
            delete m_cmp_window;
    }

    /* initiate the backing store to render the desktop offline*/
    void init(workspace *a_workspace);

    void reset() {
        //m_space_list.clear();
    }

    void insert(const space *a_space);
    void insert(const widget *a_widget);
    void insert(const window *a_window);

    void update();

    void show() {
       if (m_cmp_window)
           m_cmp_window->show();
    }

    void hide() {
        if (m_cmp_window)
            m_cmp_window->hide();
    }

private:
    cmp_window_list_t m_window_list;
    cmp_widget_list_t m_widget_list;
    cmp_space_list_t m_space_list;

    cherry_kit::window *m_cmp_window;
    cherry_kit::item_view *m_current_preview_list;

    int m_view_width;
    int m_view_height;

    workspace *m_viewport;
};

class workspace::PrivateWorkSpace {
public:
  PrivateWorkSpace() {}
  ~PrivateWorkSpace() {
      if (m_compositor)
          delete m_compositor;
  }

  void notify_update(workspace_change_t a_change, int a_space_id);

  SpacesList m_desktop_space_list;

  QRectF m_workspace_geometry;
  QRectF m_render_box;

  int m_space_count;
  int m_current_activty_space_id;
  float m_workspace_left_margine;

  std::vector<std::string> m_default_controller_name_list;
  std::vector<workspace::workspace_change_callback_t> m_notification_listners;

  bool m_opengl_on;
  int m_screen_id;

  wp_compositor *m_compositor;
};

void wp_compositor::init(workspace *a_workspace) {
  m_cmp_window = new cherry_kit::window();
  m_cmp_window->set_window_type(cherry_kit::window::kPopupWindow);

  m_viewport = a_workspace;

  /* default thumbnail returned is 10% of the actual desktop */
  QGraphicsScene *_scene = m_viewport->scene();

  float preview_width = (m_viewport->get_base_width() / 100) * 10;
  float preview_height = (m_viewport->get_base_height() / 100) * 10;

  m_view_width = m_viewport->get_base_width();
  m_view_height = m_viewport->get_base_height();

  m_current_preview_list = new cherry_kit::item_view(
      m_cmp_window, cherry_kit::item_view::kGridModel);

  m_current_preview_list->set_column_count(11);
  m_current_preview_list->set_content_size(preview_width, preview_height);
  m_current_preview_list->set_view_geometry(
      QRectF(0, 0, m_view_width,
             (preview_height + m_cmp_window->window_title_height()) - 16 ));

  m_cmp_window->set_window_opacity(0.5);

  // cleanup
  m_current_preview_list->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    a_item->view()->setParentItem(0);

    m_viewport->scene()->removeItem(a_item->view());

    if (a_item && a_item->view()) {
        delete a_item;
    }
  });

  // insert button
  cherry_kit::model_view_item *button_item = new cherry_kit::model_view_item();
  cherry_kit::icon_button *btn = new cherry_kit::icon_button();

  btn->set_contents_geometry(0, 0, 128, 128);
  btn->set_icon("navigation/ck_add.png");

  btn->on_click([=]() {
      //todo
      m_viewport->add_default_space();
  });

  button_item->set_view(btn);
  m_current_preview_list->insert(button_item);

  m_cmp_window->set_window_content(m_current_preview_list);

  _scene->addItem(m_cmp_window);
  m_cmp_window->setPos(0, 0);

  m_viewport->on_change([this](workspace::workspace_change_t type, int id) {
      m_cmp_window->hide();
  });

  hide();
}

void wp_compositor::insert(const space *a_space) {
  m_space_list.push_back(a_space);
}

void wp_compositor::insert(const widget *a_widget) {
}

void wp_compositor::insert(const window *a_window) {
}

void wp_compositor::update() {
    m_current_preview_list->clear();

    // insert button
    cherry_kit::model_view_item *button_item = new cherry_kit::model_view_item();
    cherry_kit::icon_button *btn = new cherry_kit::icon_button();

    btn->set_contents_geometry(0, 0, 128, 128);
    btn->set_icon("navigation/ck_add.png");

    btn->on_click([=]() {
        //todo
        m_viewport->add_default_space();
    });

    button_item->set_view(btn);
    m_current_preview_list->insert(button_item);
    foreach(const space *a_ptr, m_viewport->current_spaces()) {
        if (!a_ptr)
            return;

        cherry_kit::model_view_item *item = new cherry_kit::model_view_item();
        cherry_kit::image_view *image_view = new cherry_kit::image_view();

        QPixmap preview = a_ptr->preview(10.0);

        image_view->on_click([=]() {
            m_viewport->expose(a_ptr->id());
        });

        image_view->set_pixmap(preview);
        image_view->set_contents_geometry(0, 0, m_view_width / 10,
                                          m_view_height / 10);

        item->set_view(image_view);

        m_current_preview_list->insert(item);
    }
}

/* start workspace class impl */
void workspace::set_workspace_geometry(int a_screen_id) {
  QRect _current_desktop_geometry =
      QRect(0, 0, screen::get()->x_resolution(a_screen_id),
            screen::get()->y_resolution(a_screen_id));
#ifdef Q_OS_WIN
  qDebug() << Q_FUNC_INFO << "Screen : " << _current_desktop_geometry;
  setGeometry(_current_desktop_geometry);
#else
  setGeometry(_current_desktop_geometry);
#endif
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
  //setCacheMode(CacheBackground);

#ifndef Q_OS_WIN
#ifdef __QT5_TOOLKIT__
  setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnBottomHint | Qt::NoDropShadowWindowHint);
#endif

#if defined(__QT4_TOOLKIT__) && defined(__APPLE__)
  setWindowFlags(Qt::CustomizeWindowHint
		  | Qt::FramelessWindowHint
		  | Qt::WindowStaysOnBottomHint);
#endif

#endif

#ifdef Q_OS_WIN
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

  /* set the desktop geometry */
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

  setRenderHints(QPainter::Antialiasing
		  | QPainter::SmoothPixmapTransform
		  | QPainter::TextAntialiasing
                  );

  scale(width_factor, height_factor);
}

void workspace::add_default_controller(const std::string &a_controller_name) {
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

space *workspace::get_space(int a_id)
{
    return priv->m_desktop_space_list.at(a_id);
}

void workspace::set_accelerated_rendering(bool a_on) {
  priv->m_opengl_on = a_on;

  if (priv->m_opengl_on) {
    QGLFormat _gl_options(
	QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel | QGL::Rgba);
    _gl_options.setSamples(4);
    _gl_options.setAlpha(true);
    _gl_options.setAccum(true);
    QGLWidget *_gl_widget = new QGLWidget(_gl_options);

    setViewport(_gl_widget);

    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  } else {
    setupViewport(new QWidget);
    //setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //setOptimizationFlag(QGraphicsView::DontClipPainter);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  }
}

bool workspace::is_accelerated_rendering_on() const {
  return priv->m_opengl_on;
}

void workspace::auto_switch() {
#ifdef __QT5_TOOLKIT__
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
#endif
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
    p.setClipping(true);
    p.save();
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

bool workspace::event(QEvent *a_event)
{
    switch (a_event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *k = (QKeyEvent *)a_event;
            QGraphicsView::keyPressEvent(k);
            if (k->key() == Qt::Key_Backtab
                || (k->key() == Qt::Key_Tab && (k->modifiers() & Qt::ShiftModifier))
                || (k->key() == Qt::Key_Tab) ) {
                a_event->accept();
                QGraphicsItem *item = scene()->focusItem();

                if (item) {
                   item->clearFocus();
                   widget *_current_widget = dynamic_cast<widget*>(item);
                   if (_current_widget && _current_widget->focus_buddy()) {
                      _current_widget->focus_buddy()->setFocus(
                                  Qt::TabFocusReason);
                   }
                }
            }
            return true;
        }
        default:
            return QGraphicsView::event( a_event );
    }
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

void workspace::on_change(workspace::workspace_change_callback_t a_callback)
{
    priv->m_notification_listners.push_back(a_callback);
}

void workspace::show_navigator() {
  if (priv->m_compositor) {
   priv->m_compositor->update();
   priv->m_compositor->show();
  }
}

void workspace::hide_navigator()
{
  if (priv->m_compositor)
    priv->m_compositor->hide();
}

void workspace::revoke_space(const QString &a_name, int a_id) {
  space *_space = create_blank_space();
  uint _new_space_id = priv->m_desktop_space_list.count();

  _space->set_id(_new_space_id);

  _space->set_workspace(this);
  _space->set_qt_graphics_scene(scene());
  _space->set_name(a_name);
  _space->set_id(_new_space_id);

  QRectF _space_geometry;
  _space_geometry.setX(0);
  _space_geometry.setY(0);

  /*
  _space_geometry.setY((priv->m_workspace_geometry.height()) -
                       priv->m_render_box.height());
                       */

  _space_geometry.setHeight(priv->m_render_box.height());
  _space_geometry.setWidth(priv->m_render_box.width() +
                           priv->m_workspace_left_margine);

  _space->setGeometry(_space_geometry);
  _space->restore_session();

  _space->hide();

  priv->m_desktop_space_list << _space;
}

space *workspace::create_blank_space() {
  space *_space = new space();

  _space->set_workspace(this);
  _space->set_qt_graphics_scene(scene());

  priv->m_space_count += 1;
  priv->m_workspace_geometry.setHeight(priv->m_render_box.height());
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

  foreach(space *_space, priv->m_desktop_space_list) {
      if (!_space)
          continue;

      if (_space->id() == a_space_id) {
       //_space->reset_focus();

       space *_active_space = current_active_space();

       if (_active_space) {
         _active_space->hide();
       }

       QRectF _space_rect = _space->geometry();
       priv->m_current_activty_space_id = a_space_id;

       expose_sub_region(_space_rect);

       _space->update_background_texture();
       _space->show();

       break;
      }
  }
  priv->m_compositor->hide();
}

space *workspace::expose_next() {
  if (priv->m_current_activty_space_id == priv->m_desktop_space_list.count()) {
    qDebug() << Q_FUNC_INFO << "Maximum Space Count Reached";
    return 0;
  }

  space *_active_space = current_active_space();

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

  if (_active_space)
    _active_space->hide();

  _next_space->update_background_texture();
  _next_space->show();

  priv->m_compositor->hide();
  return _next_space;
}

space *workspace::expose_previous() {
  QRectF _space_geometry;
  space *_active_space = current_active_space();

  priv->m_current_activty_space_id = priv->m_current_activty_space_id - 1;

  if (priv->m_current_activty_space_id < 0) {
    priv->m_current_activty_space_id = 0;
  }

  space *_prev_space =
      priv->m_desktop_space_list.at(priv->m_current_activty_space_id);

  if (!_prev_space) {
    return 0;
  }

  if (_active_space)
    _active_space->hide();

  _prev_space->update_background_texture();
  _prev_space->show();

  priv->m_compositor->hide();
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

std::string workspace::workspace_instance_name() {
  std::string workspace_name =
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

  /* avoid deleting the last space */
  qDebug() << Q_FUNC_INFO << "-- Space Count : " << space_count();
  if (space_count() == 1) {
      return;
  }

  QString _space_ref = a_space_ptr->session_name();
  int _space_id = a_space_ptr->id();

  priv->m_workspace_geometry.setHeight(priv->m_workspace_geometry.height() -
                                       a_space_ptr->geometry().height());

  //setSceneRect(priv->m_workspace_geometry);

  priv->m_current_activty_space_id = priv->m_current_activty_space_id - 1;

  if (priv->m_current_activty_space_id < 0) {
    priv->m_current_activty_space_id = 0;
  }

  priv->m_space_count = priv->m_space_count - 1;


  space *_active_space = current_active_space();

  if (_active_space)
      _active_space->show();

  a_space_ptr->hide();

  // adjust geometry of the remaining spaces.
  //update_space_geometry(a_space_ptr, _deleted_geometry);

  // update the removed spaces into session
  save_space_removal_session_data(_space_ref);

  priv->m_desktop_space_list.removeAll(a_space_ptr);

  priv->notify_update(kSpaceRemovedNotify, _space_id);

  delete a_space_ptr;
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

  /* reset focus of the current space */
  space *_active_space = current_active_space();

  if (_active_space) {
      _active_space->reset_focus();
  }

  _space->set_name("default");

  uint _new_space_id = priv->m_desktop_space_list.count();

  _space->set_id(_new_space_id);

  std::for_each(std::begin(priv->m_default_controller_name_list),
                std::end(priv->m_default_controller_name_list),
                [=](std::string &a_controller_name) {
                  _space->add_controller(
                      QString::fromStdString(a_controller_name));
                });

  priv->m_desktop_space_list << _space;

  QRectF _space_geometry;
  _space_geometry.setX(0);
  _space_geometry.setY(0);

  /*
  _space_geometry.setY((priv->m_workspace_geometry.height()) -
                       priv->m_render_box.height());
                       */

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

  priv->notify_update(kSpaceAddedNotify, _space->id());
}

void workspace::restore_session() {
  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync(workspace_instance_name());
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      QString _space_name = a_object.property("name").c_str();
      revoke_space(_space_name, a_object.key());
    }
  });

  sync->find("Space", "", "");

  delete sync;

  priv->notify_update(kSpaceAddedNotify, 0);

  priv->m_compositor = new wp_compositor;
  priv->m_compositor->init(this);
}

float workspace::get_base_width() {
  return screen::get()->desktop_width(priv->m_screen_id);
}

float workspace::get_base_height() {
  return screen::get()->desktop_height(priv->m_screen_id);
}

void workspace::PrivateWorkSpace::notify_update(workspace_change_t a_change, int a_space_id) {
  std::for_each(std::begin(m_notification_listners),
                std::end(m_notification_listners), [=](workspace_change_callback_t a_callback) {
      if(a_callback)
          a_callback(a_change, a_space_id);
  });
}
}
