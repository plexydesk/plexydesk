#ifndef PLEXY_VIEW_CONTROLLER_PLUGIN_H
#define PLEXY_VIEW_CONTROLLER_PLUGIN_H

#define QT_SHAREDPOINTER_TRACK_POINTERS 1
#include <plexy.h>
#include <QSharedPointer>

#include <datasource.h>
#include <desktopactivity.h>
#include <space.h>
#include <plexydesk_ui_exports.h>
#include <QAction>
#include <syncobject.h>

class QGraphicsItem;
class QDropEvent;

/*!
\class UI::ViewController

\brief Base class for implementing viewport controllers for PlexyDesk.

\fn UI::ViewControllerPlugin::actions

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
UI::ViewControllerPlugin::visibleActions method and returning the string
lables for your action.
once you complete the user action remember to emit
UI::ViewControllerPlugin::actionCompleted signal
so that the action requester can notify the user about what happened to the
action.

\returns A list of action label supported by the widget  plugin
*/

namespace CherryKit {
class Widget;
class SessionSync;
typedef QList<QAction *> ActionList;

class DECL_UI_KIT_EXPORT ViewController : public QObject {
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
  virtual void set_view_rect(const QRectF &a_rect) = 0;

  /**
      * @brief
      *
      * @param view
      */
  void set_viewport(Space *a_view_ptr);

  /**
      * @brief This method returns the current viewport of the controller. this
      * will be set
      * when the controller is loaded by the loader or the desktopview.
      *
      * @return UI::AbstractDesktopView return the current viewport of
      *  the controller
      */
  virtual Space *viewport() const;
  /**
      * @brief
      *
      * @param args
      */
  virtual void
  session_data_available(const ck::sync_object &a_root_obj) = 0;
  virtual void submit_session_data(ck::sync_object *a_root_obj) = 0;

  void start_session(
      const std::string &a_session_name, const QVariantMap &a_data,
      bool a_restore,
      std::function<void(ViewController *, SessionSync *)> a_callback);
  virtual std::string
  session_database_name(const std::string &a_session_name) const;
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
  virtual void request_action(const QString &a_actionName,
                              const QVariantMap &a_args = QVariantMap());

  /**
      * @brief
      *
      * @param widget
      * @param event
      */
  virtual void handle_drop_event(Widget *a_widget_ptr, QDropEvent *a_event_ptr);

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
  virtual bool remove_widget(Widget *a_widget_ptr);

  virtual void insert(Window *a_window_ptr);

  /**
      * @brief
      *
      * @param name
      */
  virtual void set_controller_name(const QString &a_name);

  /**
      * @brief
      *
      * @return QString
      */
  virtual QString controller_name() const;

  virtual QString icon() const = 0;

  virtual QString label() const;

  virtual void configure(const QPointF &a_pos);

  virtual void prepare_removal();

protected:
  /**
      * @brief
      *
      * @param parent
      */
  explicit ViewController(QObject *a_parent_ptr = 0);
  virtual void revoke_previous_session(
      const std::string &a_session_object_name,
      std::function<void(ViewController *, SessionSync *)> a_callback);
  virtual void write_session_data(const std::string &a_session_name);
  virtual int session_count();

  /**
      * @brief
      *
      * @param source
      * @return bool
      */
  virtual bool connect_to_data_source(const QString &a_source);

Q_SIGNALS:
  /**
      * @brief
      *
      * @param source Emits the DataSource when the controller is ready
      * \sa onReady()
      */
  void data(const DataSource *a_source_ptr);

private
Q_SLOTS:
  /**
      * @brief
      *
      */
  virtual void on_ready();

private:
  class PrivateViewControllerPlugin;
  PrivateViewControllerPlugin *const o_view_controller;
};

/**
    * @brief
    *
    */
typedef QSharedPointer<ViewController> ViewControllerPtr;
}
#endif
