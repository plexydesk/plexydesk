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

namespace UI {

class UIWidget::PrivateDesktopWidget {
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
  UIWidget::WindowState m_current_widget_state;
  QPixmap m_fallback_background_pixmap;
  QPropertyAnimation *m_property_animation_for_zoom_ptr;
  QGraphicsDropShadowEffect *m_shadow_effect_ptr;
  QString m_current_window_title_str;
  WindowButton *m_window_close_button_widget_ptr;
};

UIWidget::UIWidget(QGraphicsObject *parent)
    : Window(parent), d(new PrivateDesktopWidget) {
  d->m_enable_edit_mode = false;
  d->m_enable_background_texture = false;
  d->m_current_widget_state = kRenderAsMinimizedWindow;
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

UIWidget::~UIWidget() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void UIWidget::paintView(QPainter *painter, const QRectF &rect) {
  if (!d->m_enable_background_texture)
    return;

  StyleFeatures feature;
  feature.geometry = rect;
  if (d->m_enable_window_title)
    feature.text_data = d->m_current_window_title_str;

  if (style()) {
    style()->draw("window_frame", feature, painter);
  }
}

void UIWidget::pressHoldTimeOut() {
  d->m_enable_edit_mode = true;
  update();
}

void UIWidget::windowCloseButtonClicked() { this->windowButtonAction(); }

void UIWidget::zoomDone() {
  prepareGeometryChange();
  resetMatrix();
}

void UIWidget::propertyAnimationForZoomDone() {
  if (state() == kRenderAsWindow) {
    setState(kRenderAsMinimizedWindow);
  } else {
    setState(kRenderAsWindow);
  }

  zoomDone();
}

void UIWidget::setWindowTitle(const QString &title) {
  d->m_current_window_title_str = title;
  update();
}

QString UIWidget::windowTitle() const { return d->m_current_window_title_str; }

void UIWidget::windowButtonAction() { Q_EMIT closed(this); }

void UIWidget::setWindowFlag(int flags, bool enable) {
  if (flags == kRenderBackground) {
    d->m_enable_background_texture = enable;
    return;
  }

  if (flags == kDockWindowType) {
    d->m_enable_dock_window_type = enable;
    return;
  }

  if (flags == kRenderDropShadow) {
    if (enable) {
      d->m_shadow_effect_ptr->setXOffset(0);
      d->m_shadow_effect_ptr->setYOffset(0);
      if (style()) {
        d->m_shadow_effect_ptr->setBlurRadius(
            style()->attrbute("frame", "window_blur_radius").toFloat());
      }
    } else {
      d->m_shadow_effect_ptr->setXOffset(0);
      d->m_shadow_effect_ptr->setYOffset(0);
      d->m_shadow_effect_ptr->setBlurRadius(0);
    }
    return;
  }

  if (flags == kTopLevelWindow) {
    d->m_enable_top_level_window_type = enable;
    return;
  }

  if (flags == kRenderWindowTitle) {
    d->m_enable_window_title = enable;
    return;
  }

  if (flags == kConvertToWindowType) {
    d->m_enable_window_type = enable;
    /*
    if (enable)
      d->m_window_close_button_widget_ptr->show();
    else
      d->m_window_close_button_widget_ptr->hide();
      */
    return;
  }
}

void UIWidget::updateWindowButton(int zValue) {
  //d->m_window_close_button_widget_ptr->setZValue(zValue + 1);
}

UIWidget::WindowState UIWidget::state() { return d->m_current_widget_state; }

void UIWidget::setState(UIWidget::WindowState s) {
  d->m_current_widget_state = s;
  show();
}

void UIWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  d->m_press_hold_timer_ptr->stop();
  Q_EMIT clicked();
  Window::mouseReleaseEvent(event);
}

StylePtr UIWidget::style() const { return Theme::style(); }

void UIWidget::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (d->m_enable_edit_mode) {
    d->m_press_hold_timer_ptr->stop();
    Q_EMIT closed(this);
    // QGraphicsItem::mousePressEvent(event);
    return;
  }

  if (event->buttons() == Qt::LeftButton && state() == kRenderAsWindow) {
    d->m_press_hold_timer_ptr->start(3000);
    Window::mousePressEvent(event);
    return;
  }
  Window::mousePressEvent(event);
}

void UIWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  if (!d->m_enable_dock_window_type || !d->m_enable_top_level_window_type) {
    QGraphicsItem::mouseDoubleClickEvent(event);
    return;
  }

  if (event->buttons() != Qt::LeftButton) {
    /* We will let the widgets decide what to do with
       left clicks
       */
    QGraphicsItem::mouseDoubleClickEvent(event);
    return;
  }
  if (state() == UIWidget::kRenderAsWindow) {
    prepareGeometryChange();
    // TODO: Use Animation forward and back methods
    d->m_property_animation_for_zoom_ptr->setPropertyName("rect");
    d->m_property_animation_for_zoom_ptr->setDuration(200);
    QRectF startRect(
        (geometry().width() / 2) - (minimizedGeometry().width() / 2),
        (geometry().height() / 2) - (minimizedGeometry().height() / 2),
        minimizedGeometry().width(), minimizedGeometry().height());
    d->m_property_animation_for_zoom_ptr->setStartValue(startRect);
    d->m_property_animation_for_zoom_ptr->setEndValue(geometry());
    d->m_property_animation_for_zoom_ptr->setEasingCurve(QEasingCurve::InQuart);

    d->m_property_animation_for_zoom_ptr->start();
    setChildWidetVisibility(true);
    /*
    if (!d->m_enable_window_type)
      d->m_window_close_button_widget_ptr->hide();
      */

  } else {
    prepareGeometryChange();
    d->m_property_animation_for_zoom_ptr->setPropertyName("rect");
    d->m_property_animation_for_zoom_ptr->setDuration(100);

    QRectF endRect((geometry().width() / 2) - (minimizedGeometry().width() / 2),
                   (geometry().height() / 2) -
                       (minimizedGeometry().height() / 2),
                   minimizedGeometry().width(), minimizedGeometry().height());
    d->m_property_animation_for_zoom_ptr->setEndValue(endRect);
    d->m_property_animation_for_zoom_ptr->setStartValue(geometry());
    d->m_property_animation_for_zoom_ptr->setEasingCurve(
        QEasingCurve::OutQuart);
    this->setVisible(true);
    d->m_property_animation_for_zoom_ptr->start();
    setChildWidetVisibility(false);
  }
  QGraphicsItem::mouseDoubleClickEvent(event);
}
}
