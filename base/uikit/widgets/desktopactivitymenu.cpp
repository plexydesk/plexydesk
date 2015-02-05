#include "desktopactivitymenu.h"
#include <space.h>
#include <QDebug>
#include <QTimer>
#include <workspace.h>

namespace UI
{

class DesktopActivityMenu::PrivateActivityPopup
{
public:
  PrivateActivityPopup() {}

  ~PrivateActivityPopup() {}

  Space *m_owner_space;
  DesktopActivityPtr m_desktop_activity;
  bool m_current_visibility;
};

DesktopActivityMenu::DesktopActivityMenu(QObject *parent)
  : QObject(parent), d(new PrivateActivityPopup)
{
  d->m_owner_space = 0;
  d->m_current_visibility = false;
}

UI::DesktopActivityMenu::~DesktopActivityMenu()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void DesktopActivityMenu::setActivity(DesktopActivityPtr activity)
{
  d->m_desktop_activity = activity;
  connect(activity->window(), SIGNAL(focusLost()), this,
          SLOT(onFocusOutEvent()));
}

void DesktopActivityMenu::setSpace(Space *space)
{
  // if we have a space we will not rest this.
  // create a popup for each space. sharing between
  // spaces are not possible at the moment.
  if (!d->m_owner_space) {
    d->m_owner_space = space;
  }
}

void DesktopActivityMenu::show()
{
  if (d->m_desktop_activity) {
    if (d->m_owner_space) {
      d->m_owner_space->addActivity(d->m_desktop_activity);
    }

    WorkSpace *_w = qobject_cast<WorkSpace *>(d->m_owner_space->workspace());

    d->m_desktop_activity->showActivity();
    d->m_desktop_activity->window()->setZValue(100000);
    d->m_desktop_activity->window()->setFocus(Qt::MouseFocusReason);
    d->m_desktop_activity->window()->show();
    qDebug() << Q_FUNC_INFO << "Showing Activity Window visibility : "
             << d->m_desktop_activity->window()->isVisible();
    qDebug() << Q_FUNC_INFO << "Showing Activity Window Pos : "
             << d->m_desktop_activity->window()->pos();
    qDebug() << Q_FUNC_INFO
             << "Showing Activity on : " << d->m_owner_space->id();
    qDebug() << Q_FUNC_INFO << "Showing Activity on Geometry : "
             << d->m_owner_space->geometry();

    qDebug() << Q_FUNC_INFO
             << "Showing Activity on Geometry Space : " << _w->sceneRect();

    d->m_current_visibility = true;
    Q_EMIT activated();
  } else {
    qDebug() << Q_FUNC_INFO << "Activity Not Set";
  }
}

void DesktopActivityMenu::hide()
{
  if (d->m_desktop_activity) {
    d->m_desktop_activity->window()->hide();
    d->m_current_visibility = false;
    Q_EMIT deactivated();
  }
}

void DesktopActivityMenu::exec(const QPointF &pos)
{
  if (d->m_desktop_activity) {
    d->m_desktop_activity->exec(pos);
    d->m_current_visibility = true;
    show();
  }
}

void DesktopActivityMenu::onFocusOutEvent()
{
  QTimer::singleShot(250, this, SLOT(lateHide()));
}

void DesktopActivityMenu::lateHide() { hide(); }
}
