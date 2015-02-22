#ifndef PD_SPACE_H
#define PD_SPACE_H

#include <QGraphicsItem>
#include <QObject>

#include <view_controller.h>
#include <plexydesk_ui_exports.h>
#include <widget.h>

namespace UIKit
{
class WorkSpace;

class DECL_UI_KIT_EXPORT Space : public QObject
{
  Q_OBJECT
public:
  typedef enum {
    kCenterOnViewport,
    kCenterOnViewportLeft,
    kCenterOnViewportRight,
    kCenterOnViewportTop,
    kCenterOnViewportBottom
  } ViewportLocation;

  explicit Space(QObject *aParent = 0);
  virtual ~Space();

  virtual void addController(const QString &aName);
  virtual ViewControllerPtr controller(const QString &aName);
  virtual QStringList currentControllerList() const;

  virtual void setName(const QString &aName);
  virtual QString name() const;

  virtual void setId(int aId);
  virtual int id() const;

  virtual QObject *workspace();
  virtual void setWorkspace(WorkSpace *aWorkspace);

  virtual void restoreSession();

  virtual void setScene(QGraphicsScene *aScene);

  virtual void setGeometry(const QRectF &aRealRect);
  virtual QRectF geometry() const;

  QString sessionName() const;
  virtual void updateSessionValue(const QString &aName,
                                  const QString &aKey,
                                  const QString &aValue);

  virtual void addActivity(UIKit::DesktopActivityPtr aActivity);
  UIKit::DesktopActivityPtr createActivity(const QString &aActivity,
                                        const QString &aTitle,
                                        const QPointF &aPos,
                                        const QRectF &aRect,
                                        const QVariantMap &aDataMap);

  virtual QPointF mousePointerPos() const;
  virtual QPointF center(const QRectF &aViewGeometry,
                         const ViewportLocation&aLocation = kCenterOnViewport
          ) const;

  virtual void handleDropEvent(QDropEvent *event, const QPointF &event_pos);

Q_SIGNALS:
  void controllerAdded(const QString &aName);

public Q_SLOTS:
  virtual void addWidgetToView(Widget *aWidget);
  virtual void onWidgetClosed(Widget *aWidget);
  virtual void onActivityFinished();

protected:
  virtual void clear();
  void registerController(const QString &aControllerName);

private:
  class PrivateSpace;
  PrivateSpace *const mPrivImpl;
};
}
#endif // SPACE_H
