#ifndef PD_SPACE_H
#define PD_SPACE_H

#include <QGraphicsItem>
#include <QObject>
#include <desktopactivitymenu.h>

#include <view_controller.h>
#include <desktop_viewport.h>

#include <plexydesk_ui_exports.h>

namespace PlexyDesk {

class WorkSpace;

class DECL_UI_KIT_EXPORT Space : public PlexyDesk::DesktopViewport {
  Q_OBJECT
public:
  explicit Space(QObject *parent = 0);

  virtual ~Space();

  virtual void addController(const QString &controllerName);

  virtual void setName(const QString &name);

  virtual QString name() const;

  virtual void setId(int id);

  virtual int id() const;

  virtual QObject *workspace();

  virtual void setWorkspace(WorkSpace *workspace);

  virtual void restoreSession();

  virtual void setScene(PlatformGraphicsScene *scene);

  virtual void setSpaceGeometry(const QRectF &rectf);

  virtual QRectF geometry() const;

  virtual void handleDropEvent(QDropEvent *event, const QPointF &event_pos);
  /**
      * @brief
      *
      * @param name
      * @return ControllerPtr
      */
  virtual ControllerPtr controller(const QString &name);
  /**
      * @brief
      *
      * @return QStringList
      */
  virtual QStringList currentDesktopControllers() const;
  /**
      * @brief
      *
      * @param controllerName
      * @param key
      * @param value
      */
  virtual void updateSessionValue(const QString &controllerName,
                                  const QString &key, const QString &value);
  /**
      * @brief
      *
      * @param activity
      */
  virtual void addActivity(DesktopActivityPtr activity);

  virtual void addActivityPoupToView(QSharedPointer<DesktopActivityMenu> menu);

  DesktopActivityPtr createActivity(const QString &activity,
                                    const QString &title, const QPointF &pos,
                                    const QRectF &rect,
                                    const QVariantMap &dataItem);
  QString sessionName() const;

  virtual void clear();

  virtual void closeMenu();

  virtual QPointF clickLocation() const;

public Q_SLOTS:
  virtual void addWidgetToView(Widget *widget);
  virtual void onWidgetClosed(PlexyDesk::Widget *widget);
  virtual void onActivityFinished();

private Q_SLOTS:
  void onControllerAdded(const QString &controllerName);

private:
  class PrivateSpace;
  PrivateSpace *const d;
};
}
#endif // SPACE_H
