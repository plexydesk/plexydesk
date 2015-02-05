#include "menu.h"
#include <space.h>
#include <QDebug>
#include <QTimer>

namespace UI
{

class Menu::PrivateMenu
{
public:
  PrivateMenu() {}

  ~PrivateMenu()
  {
    if (m_desktop_widget) {
      m_desktop_widget.clear();
    }
  }

  QSharedPointer<Widget> m_desktop_widget;
  bool m_current_visibility;
};

Menu::Menu(QObject *parent) : QObject(parent), d(new PrivateMenu)
{
  d->m_current_visibility = false;
}

UI::Menu::~Menu()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void Menu::setContentWidget(QSharedPointer<Widget> widget)
{
  d->m_desktop_widget = widget;
  widget->hide();
  connect(widget.data(), SIGNAL(focusLost()), this, SLOT(onFocusOutEvent()));
}

void Menu::show()
{
  if (d->m_desktop_widget) {
    d->m_desktop_widget->show();
    d->m_desktop_widget->setFocus(Qt::MouseFocusReason);
    d->m_current_visibility = true;
    Q_EMIT activated();
  } else {
    qDebug() << Q_FUNC_INFO << "Activity Not Set";
  }
}

void Menu::hide()
{
  if (d->m_desktop_widget) {
    d->m_desktop_widget->hide();
    d->m_current_visibility = false;
    Q_EMIT deactivated();
  }
}

void Menu::exec(const QPointF &pos)
{
  if (d->m_desktop_widget) {
    d->m_desktop_widget->setPos(pos);
    d->m_current_visibility = true;
    show();
  } else {
    qDebug() << Q_FUNC_INFO << "Error: Null Widget Set";
  }
}

void Menu::onFocusOutEvent() { QTimer::singleShot(250, this, SLOT(hide())); }
}
