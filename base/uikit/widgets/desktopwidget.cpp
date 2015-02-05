#include "desktopwidget.h"

#include <QCoreApplication>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QTimeLine>
#include <QTimer>
#include <QDir>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

#include <imagecache.h>
#include <svgprovider.h>
#include <windowbutton.h>
#include <themepackloader.h>

namespace UI
{

class UIWidget::PrivateDesktopWidget
{
public:
  PrivateDesktopWidget() {}
  ~PrivateDesktopWidget() {}

  bool m_enable_background_texture;
  bool m_enable_dock_window_type;
  bool m_enable_window_type;
  bool m_enable_edit_mode;
  bool m_enable_top_level_window_type;
  bool m_enable_window_title;
  QTimer *m_press_hold_timer_ptr;
  QPixmap m_fallback_background_pixmap;
  QPropertyAnimation *m_property_animation_for_zoom_ptr;
  QGraphicsDropShadowEffect *m_shadow_effect_ptr;
  QString m_current_window_title_str;
  WindowButton *m_window_close_button_widget_ptr;
};

UIWidget::UIWidget(QGraphicsObject *parent)
  : Window(parent), d(new PrivateDesktopWidget)
{
  d->m_enable_edit_mode = false;
  d->m_enable_background_texture = false;
  d->m_enable_dock_window_type = true;
  d->m_enable_window_type = false;
  d->m_enable_top_level_window_type = false;
  d->m_enable_window_title = false;

  d->m_property_animation_for_zoom_ptr = new QPropertyAnimation(this);
  d->m_property_animation_for_zoom_ptr->setTargetObject(this);

  connect(d->m_property_animation_for_zoom_ptr, SIGNAL(finished()), this,
          SLOT(propertyAnimationForZoomDone()));

  setCacheMode(DeviceCoordinateCache);
  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  setAcceptHoverEvents(true);

  // presshold
  d->m_press_hold_timer_ptr = new QTimer(this);
  connect(d->m_press_hold_timer_ptr, SIGNAL(timeout()), this,
          SLOT(pressHoldTimeOut()));

  // dropshadow
  d->m_shadow_effect_ptr = new QGraphicsDropShadowEffect(this);
  d->m_shadow_effect_ptr->setBlurRadius(0);
  d->m_shadow_effect_ptr->setXOffset(0);
  d->m_shadow_effect_ptr->setYOffset(0);

  if (style()) {
    d->m_shadow_effect_ptr->setColor(
      QColor(style()->attrbute("frame", "window_shadow_color").toString()));
  }

  this->setGraphicsEffect(d->m_shadow_effect_ptr);

  // window buttons
  //d->m_window_close_button_widget_ptr = new WindowButton(this);
  //d->m_window_close_button_widget_ptr->hide();

  if (style()) {
    setMinimizedGeometry(
      QRect(0.0, 0.0,
            style()->attrbute("frame", "window_minimized_width").toFloat(),
            style()->attrbute("frame", "window_minimized_height").toFloat()));
    /*
    d->m_window_close_button_widget_ptr->setPos(
        style()->attrbute("frame", "padding").toFloat(),
        style()->attrbute("frame", "padding").toFloat());
        */
  }

  //connect(d->m_window_close_button_widget_ptr, SIGNAL(clicked()), this,
  //       SLOT(windowCloseButtonClicked()));
}

UIWidget::~UIWidget()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void UIWidget::pressHoldTimeOut()
{
  d->m_enable_edit_mode = true;
  update();
}

void UIWidget::windowCloseButtonClicked() {  }

void UIWidget::zoomDone()
{
  prepareGeometryChange();
  resetMatrix();
}

void UIWidget::propertyAnimationForZoomDone()
{
  zoomDone();
}


StylePtr UIWidget::style() const { return Theme::style(); }

}
