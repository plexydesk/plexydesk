#ifndef PLEXY_VIEW_CONTROLLER_PLUGIN_H
#define PLEXY_VIEW_CONTROLLER_PLUGIN_H

#define QT_SHAREDPOINTER_TRACK_POINTERS 1
#include <plexy.h>
#include <QSharedPointer>

#include <datasource.h>
#include <desktopactivity.h>
#include <desktop_viewport.h>
#include <plexy_core_exports.h>
#include <QAction>

class QGraphicsItem;
class QDropEvent;

/*!
\class PlexyDesk::ViewController

\brief Base class for implementing viewport controllers for PlexyDesk.

\fn PlexyDesk::ViewControllerPlugin::actions

\brief Actions supported by the plugin

\paragraph Widget Plugins can optionaly provide actions which
it can perform on the view and the data source. So ViewControllerPlugins
mostly act as a controller which knows about the data and the view
and provides suitable actions which the user can perform.

\paragraph If your plugin doesn't provide any actions which the user can
execute, for instance a dash board widget plugin which displays read-only
data source doesn't need to provide any user actions. In such cases plugin
authors should not worry about overiding this method in their plugin class.

\paragraph But if you plugin needs user actions such as adding a clock with a
different
Time Zone, then you can return the actions you want to perform by overiding
PlexyDesk::ViewControllerPlugin::visibleActions method and returning the string
lables for your action.
once you complete the user action remember to emit
PlexyDesk::ViewControllerPlugin::actionCompleted signal
so that the action requester can notify the user about what happened to the
action.

\returns A list of action label supported by the widget  plugin
*/

namespace PlexyDesk {
class Widget;
typedef QList<QAction *> ActionList;

class CORE_DECL_EXPORT ViewController : public QObject {
  Q_OBJECT

public:
  /**
      * @brief
      *
      */
  virtual ~ViewController();
    /**
      * @brief once the controller is loaded this method will be called by the
      * loader. initilisization of the controller has to be done here. by
      * inheriting
      * this class.
      *
      */
  virtual void init() = 0;
  /**
      * @brief
      *
      * @param rect
      */
  virtual void setViewRect(const QRectF &rect) = 0;

  /**
      * @brief
      *
      * @param view
      */
  void setViewport(DesktopViewport *view);

  /**
      * @brief This method returns the current viewport of the controller. this
      * will be set
      * when the controller is loaded by the loader or the desktopview.
      *
      * @return PlexyDesk::AbstractDesktopView return the current viewport of
      *  the controller
      */
  virtual DesktopViewport *viewport();
  /**
      * @brief
      *
      * @param args
      */
  virtual void revokeSession(const QVariantMap &args) = 0;

  /**
      * @brief
      *
      * @return QStringList
      */
  virtual ActionList actions() const;

  /**
      * @brief
      *
      * @param actionName
      * @param args
      */
  virtual void requestAction(const QString &actionName,
                             const QVariantMap &args = QVariantMap());

  /**
      * @brief
      *
      * @param widget
      * @param event
      */
  virtual void handleDropEvent(Widget *widget, QDropEvent *event);

  /**
      * @brief
      *
      * @return DataSource
      */
  virtual DataSource *dataSource();

  /**
      * @brief
      *
      * @param widget
      * @return bool
      */
  virtual bool removeWidget(Widget *widget);

  virtual void insert(Widget *widget);

  /**
      * @brief
      *
      * @param name
      */
  virtual void setControllerName(const QString &name);

  /**
      * @brief
      *
      * @return QString
      */
  virtual QString controllerName() const;

  /**
      * @brief Method used to get the activity pointer by name
      *
      * @param name The idenfier name of the desktop activity
      * @return DesktopActivityPtr Returns a Activity Pointer
      */
  virtual DesktopActivityPtr activity(const QString &name,
                                      const QRectF &geometry = QRectF(),
                                      const QPointF &pos = QPoint(0.0, 0.0),
                                      const QString &window_title = QString());

  virtual QString icon() const = 0;

  virtual QString label() const;

  virtual void configure(const QPointF &pos);

  virtual void prepareRemoval();

protected:
  /**
      * @brief
      *
      * @param parent
      */
  explicit ViewController(QObject *parent = 0);

  /**
      * @brief
      *
      * @param source
      * @return bool
      */
  virtual bool connectToDataSource(const QString &source);

Q_SIGNALS:
  /**
      * @brief
      *
      * @param source Emits the DataSource when the controller is ready
      * \sa onReady()
      */
  void data(const DataSource *source);


private Q_SLOTS:
  /**
      * @brief
      *
      */
  virtual void onReady();

private:
  class PrivateViewControllerPlugin;
  PrivateViewControllerPlugin *const d;
};

/**
    * @brief
    *
    */
typedef QSharedPointer<ViewController> ControllerPtr;
}
#endif
