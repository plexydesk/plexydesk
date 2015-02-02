#include "toolbar.h"

#include <imagebutton.h>
#include <themepackloader.h>
#include <QDesktopWidget>
#include <QGraphicsWidget>
#include <tableview.h>
#include <QGraphicsLinearLayout>

namespace UI {
class ToolBar::PrivateToolBar {
public:
  PrivateToolBar() {}
  ~PrivateToolBar() {}

  QRectF contentGeometry() const;
  QRectF frameGeometry() const;

  uint mButtonCount;
  QGraphicsWidget *mLayoutBase;
  QGraphicsLinearLayout *mLayout;
  QSize m_icon_size;
  QString m_icon_resolution;

  QList<Widget *> m_widget_list;
};

ToolBar::ToolBar(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateToolBar) {
  d->mButtonCount = 0;
  d->m_icon_resolution = "mdpi";
  d->m_icon_size = QSize(24, 24);

  d->mLayoutBase = new QGraphicsWidget(this);
  d->mLayout = new QGraphicsLinearLayout();
  d->mLayoutBase->setLayout(d->mLayout);
  d->mLayout->setContentsMargins(5.0, 5.0, 5.0, 5.0);
  d->mLayout->setSpacing(5.0);
  d->mLayout->setOrientation(Qt::Horizontal);

  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable, false);
}

ToolBar::~ToolBar() { delete d; }

void ToolBar::addAction(const QString &lable, const QString &icon,
                        bool togleAction) {
  ImageButton *button = new ImageButton(d->mLayoutBase);

  float _button_size = d->m_icon_size.width();

  if (d->mButtonCount != 0) {
    d->mLayout->addStretch();
  }

  button->setLable(lable);
  button->setPixmap(UI::Theme::instance()->drawable(
      icon + ".png", d->m_icon_resolution));
  button->setGeometry(QRectF(QPointF(), d->m_icon_size));
  button->setMinimumSize(d->m_icon_size);

  connect(button, SIGNAL(clicked()), this, SLOT(onButtonPressed()));
  d->mLayout->addItem(button);
  d->mButtonCount = d->mButtonCount + 1;

  d->mLayout->invalidate();
  d->mLayout->updateGeometry();
  d->mLayout->activate();
}

void ToolBar::insertWidget(UIWidget *widget) {
  if (d->mLayout->count() != 0) {
    d->mLayout->addStretch();
  }

  d->mLayout->addItem(widget);
  d->mLayout->invalidate();
  d->mLayout->updateGeometry();
  d->mLayout->activate();
  update();
}

void ToolBar::setOrientation(Qt::Orientation orientation) {
  d->mLayout->setOrientation(orientation);
}

void ToolBar::setIconResolution(const QString &res) {
  d->m_icon_resolution = res;
}

void ToolBar::setIconSize(const QSize &size) { d->m_icon_size = size; }

StylePtr ToolBar::style() const { return Theme::style(); }

/*
void ToolBar::setGeometry(const QRectF &rect)
{
    qDebug() << Q_FUNC_INFO << rect;

   // d->mLayout->setGeometry(QRectF(0, 0, rect.width(), rect.height() + 400));
    //d->mLayoutBase->setPos(QPointF());
    Widget::setGeometry(rect);
}
*/

QRectF ToolBar::contentGeometry() const { return d->contentGeometry(); }

QRectF ToolBar::frameGeometry() const { return d->frameGeometry(); }

QSizeF ToolBar::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
  return d->frameGeometry().size();
}

void ToolBar::paintView(QPainter *painter, const QRectF &exposeRect) {
  QPen pen;
  painter->save();
  painter->fillRect(frameGeometry(), QColor("#f0f0f0"));
  painter->restore();
}

void ToolBar::onButtonPressed() {
  if (sender()) {
    ImageButton *button = qobject_cast<ImageButton *>(sender());
    if (button) {
      qDebug() << Q_FUNC_INFO << button->label();
      Q_EMIT action(button->label());
    }
  }
}

QRectF ToolBar::PrivateToolBar::contentGeometry() const {
  return mLayout->contentsRect();
}

QRectF ToolBar::PrivateToolBar::frameGeometry() const {
  return mLayout->geometry();
}
}
