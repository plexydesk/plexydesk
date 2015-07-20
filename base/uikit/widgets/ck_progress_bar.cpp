#include "ck_progress_bar.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>
#include <QVariantAnimation>
#include <ck_resource_manager.h>

namespace cherry_kit {

class progress_bar::PrivateProgressBar {
public:
  PrivateProgressBar() : m_width(0), m_height(0) {}
  ~PrivateProgressBar() {}

  float m_maximum_value;
  float m_minimum_value;
  float m_current_value;

  int m_width;
  int m_height;
};

progress_bar::progress_bar(widget *parent)
    : widget(parent), priv(new PrivateProgressBar) {
  priv->m_maximum_value = 100.0;
  priv->m_minimum_value = 1.0;
  priv->m_current_value = (float)0.01;
  priv->m_width = 320;
  priv->m_height = 240;
}

progress_bar::~progress_bar() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

void progress_bar::set_size(const QSizeF &size) {
  priv->m_width = size.width();
  priv->m_height = size.height();
  setGeometry(QRectF(0, 0, size.width(), size.height()));
  update();
}

/*
QSizeF progress_bar::sizeHint(Qt::SizeHint which,
                              const QSizeF &a_constraint) const {
  return QSizeF(priv->m_width, priv->m_height);
}
*/

QRectF progress_bar::contents_geometry() const {
  return QRectF(0, 0, priv->m_width, priv->m_height);
}

int progress_bar::max_range() { return priv->m_maximum_value; }

int progress_bar::min_range() { return priv->m_minimum_value; }

void progress_bar::set_range(int a_min, int a_max) {
  priv->m_minimum_value = a_min;
  priv->m_maximum_value = a_max;
  update();
}

void progress_bar::set_value(int a_value) {
  if (a_value > priv->m_maximum_value) {
    priv->m_current_value = priv->m_maximum_value;
  } else if (a_value < priv->m_minimum_value) {
    priv->m_current_value = priv->m_minimum_value;
  } else
    priv->m_current_value = a_value;
  update();
}

int progress_bar::value() const {
  float percentage = (priv->m_current_value / priv->m_maximum_value) * 100;

  return percentage;
}

void progress_bar::on_value_changed(const QVariant &a_value) {
  priv->m_current_value = a_value.toFloat();
  update();
}

void progress_bar::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  style_data features;
  features.render_state = style_data::kRenderBackground;
  features.geometry = a_rect.adjusted(10, 0, -24, 0);

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("linear_progress_bar", features,
                                                a_painter_ptr);
  }

  float percentage_completed =
      (priv->m_current_value / priv->m_maximum_value) * 100;
  float current_progress = ((a_rect.width() - 24) / 100) * percentage_completed;

  features.geometry = QRectF(10.0, 0.0, current_progress, a_rect.height());

  features.render_state = style_data::kRenderForground;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("linear_progress_bar", features,
                                                a_painter_ptr);
  }
}
}
