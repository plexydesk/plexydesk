#ifndef PD_SPACE_H
#define PD_SPACE_H

#include <QGraphicsItem>
#include <QObject>

#include <desktopactivitymenu.h>
#include <view_controller.h>
#include <plexydesk_ui_exports.h>
#include <desktopwidget.h>

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

  virtual void addController(const QString &aName);
  virtual ViewControllerPtr controller(const QString &aName);
  virtual QStringList getSessionControllers() const;

  virtual void setName(const QString &aName);
  virtual QString name() const;

  virtual void setId(int aId);
  virtual int id() const;

  virtual QObject *workspace();
  virtual void setWorkspace(WorkSpace *aWorkspace);

  virtual void restoreSession();

  virtual void setScene(QGraphicsScene *aScenePtr);

  virtual void setGeometry(const QRectF &aRealRect);
  virtual QRectF geometry() const;

  QString sessionName() const;
  virtual void updateSessionValue(const QString &aName,
                                  const QString &aKey, const QString &aValue);

  virtual void addActivity(UI::DesktopActivityPtr activity);

  UI::DesktopActivityPtr createActivity(const QString &activity,
                                    const QString &title, const QPointF &pos,
                                    const QRectF &rect,
                                    const QVariantMap &dataItem);

  virtual void addActivityPoupToView(QSharedPointer<DesktopActivityMenu> menu);
  virtual void dismissActivityPopup();

  virtual QPointF mousePointerPos() const;
  virtual QPointF center(const QRectF &viewGeometry,
                         const ViewportLocation &loc = kCenterOnViewport) const;

  virtual void handleDropEvent(QDropEvent *event, const QPointF &event_pos);

Q_SIGNALS:
  void controllerAdded(const QString &name);

public Q_SLOTS:
  virtual void addWidgetToView(UIWidget *widget);
  virtual void onWidgetClosed(UIWidget *widget);
  virtual void onActivityFinished();

private Q_SLOTS:
  void registerController(const QString &controllerName);

protected:
  virtual void clear();

private:
  class PrivateSpace;
  PrivateSpace *const d;
};
}
#endif // SPACE_H
