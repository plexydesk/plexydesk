#ifndef PD_SPACE_H
#define PD_SPACE_H

#include <QGraphicsItem>
#include <QObject>

#include <desktopactivitymenu.h>
#include <view_controller.h>
#include <plexydesk_ui_exports.h>

namespace UI {

class WorkSpace;

class DECL_UI_KIT_EXPORT Space : public QObject {
  Q_OBJECT
public:
 typedef enum {
    kCenterOnViewport,
    kCenterOnViewportLeft,
    kCenterOnViewportRight,
    kCenterOnViewportTop,
    kCenterOnViewportBottom
  } ViewportLocation;
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

  virtual void setScene(QGraphicsScene *scene);

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

  virtual QPointF center(const QRectF &viewGeometry,
                         const ViewportLocation &loc = kCenterOnViewport) const;

Q_SIGNALS:
  void controllerAdded(const QString &name);

public Q_SLOTS:
  virtual void addWidgetToView(Widget *widget);
  virtual void onWidgetClosed(UI::Widget *widget);
  virtual void onActivityFinished();

private Q_SLOTS:
  void onControllerAdded(const QString &controllerName);

private:
  class PrivateSpace;
  PrivateSpace *const d;
};
}
#endif // SPACE_H
