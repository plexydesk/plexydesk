#include <ck_desktop_controller_interface.h>
#include <ck_extension_manager.h>
#include <QDebug>

#include <datasync.h>
#include <disksyncengine.h>
#include <ck_session_sync.h>

namespace cherry_kit {

class desktop_controller_interface::PrivateViewControllerPlugin {
public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}

  QSharedPointer<data_source> m_data_source;
  space *m_viewport;
  QString m_name;

  std::vector<session_sync *> m_session_list;
};

desktop_controller_interface::desktop_controller_interface(QObject *parent)
    : QObject(parent), o_view_controller(new PrivateViewControllerPlugin) {
  o_view_controller->m_viewport = 0;
}

void desktop_controller_interface::revoke_previous_session(
    const std::string &a_session_object_name,
    std::function<void(desktop_controller_interface *, session_sync *)>
        a_callback) {
  cherry::data_sync *sync =
      new cherry::data_sync(session_database_name(a_session_object_name));
  cherry::disk_engine *engine = new cherry::disk_engine();
  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      QVariantMap session_data;

      cherry::ck_string_list prop_list = a_object.property_list();

      std::for_each(std::begin(prop_list), std::end(prop_list),
                    [&](const std::string &a_prop) {
        session_data[a_prop.c_str()] = a_object.property(a_prop).c_str();
      });

      start_session(a_session_object_name, session_data, true,
                    [&](desktop_controller_interface *a_controller,
                        cherry_kit::session_sync *a_session) {
        a_callback(a_controller, a_session);
      });
    }
  });

  sync->find(a_session_object_name, "", "");
  delete sync;
}

void desktop_controller_interface::write_session_data(
    const std::string &a_session_name) {
  std::string session_name = session_database_name(a_session_name);
  std::string key_name = a_session_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
  key_name += "_id";

  std::for_each(std::begin(o_view_controller->m_session_list),
                std::end(o_view_controller->m_session_list),
                [&](cherry_kit::session_sync *session_ref) {
    if (session_ref->is_purged())
      return;

    if (session_ref->session_group_key().compare(key_name) != 0)
      return;

    cherry::data_sync *sync = new cherry::data_sync(session_name);
    cherry::disk_engine *engine = new cherry::disk_engine();
    sync->set_sync_engine(engine);

    session_ref->update_session();
    cherry::sync_object clock_session_obj;

    clock_session_obj.set_name(a_session_name);
    Q_FOREACH(const QString & a_key, session_ref->session_keys()) {
      clock_session_obj.set_property(
          a_key.toStdString(),
          std::string(session_ref->session_data(a_key).toByteArray()));
    }

    sync->on_object_found([&](cherry::sync_object &a_object,
                              const std::string &a_app_name, bool a_found) {
      if (!a_found) {
        sync->add_object(clock_session_obj);
      } else {
        sync->save_object(clock_session_obj);
      }
    });

    sync->find(a_session_name, key_name, session_ref->session_id_to_string());
    delete sync;
  });
}

int desktop_controller_interface::session_count() {
  return o_view_controller->m_session_list.size();
}

desktop_controller_interface::~desktop_controller_interface() {
  delete o_view_controller;
}

void desktop_controller_interface::set_viewport(space *a_view_ptr) {
  o_view_controller->m_viewport = a_view_ptr;
}

space *desktop_controller_interface::viewport() const {
  return o_view_controller->m_viewport;
}

void desktop_controller_interface::start_session(
    const std::string &a_session_name, const QVariantMap &a_data,
    bool a_restore, std::function<void(desktop_controller_interface *,
                                       session_sync *)> a_callback) {
  cherry_kit::session_sync *session_ref =
      new cherry_kit::session_sync(a_session_name, a_data);
  session_ref->set_session_id(
      a_data[QString::fromStdString(a_session_name).toLower() + "_id"].toInt());

  o_view_controller->m_session_list.push_back(session_ref);

  std::function<void()> on_session_callback =
      std::bind(a_callback, this, session_ref);

  session_ref->on_session_init(on_session_callback);

  session_ref->session_init();
  if (viewport() && a_restore == false) {
    viewport()->update_session_value(controller_name(), "", "");
  }
}

std::string desktop_controller_interface::session_database_name(
    const std::string &a_session_name) const {
  std::string key_name = a_session_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

  std::string session_db_name =
      o_view_controller->m_viewport->session_name_for_controller(
                                         controller_name()).toStdString() +
      "_org." + key_name + ".data";
  return session_db_name;
}

ActionList desktop_controller_interface::actions() const {
  return ActionList();
}

void
desktop_controller_interface::request_action(const QString & /*actionName*/,
                                             const QVariantMap & /*args*/) {}

void desktop_controller_interface::handle_drop_event(widget * /*widget*/,
                                                     QDropEvent * /*event*/) {}

data_source *desktop_controller_interface::dataSource() {
  return o_view_controller->m_data_source.data();
}

void desktop_controller_interface::set_controller_name(const QString &a_name) {
  o_view_controller->m_name = a_name;
}

QString desktop_controller_interface::controller_name() const {
  return o_view_controller->m_name;
}

QString desktop_controller_interface::label() const { return QString(); }

void desktop_controller_interface::configure(const QPointF &a_pos) {
  Q_UNUSED(a_pos)
}

void desktop_controller_interface::prepare_removal() {
  o_view_controller->m_data_source.clear();
}

bool
desktop_controller_interface::connect_to_data_source(const QString &a_source) {
  o_view_controller->m_data_source =
      extension_manager::instance()->data_engine(a_source);

  if (!o_view_controller->m_data_source.data()) {
    return 0;
  }

  connect(o_view_controller->m_data_source.data(), SIGNAL(ready()), this,
          SLOT(on_ready()));

  return true;
}

bool desktop_controller_interface::remove_widget(widget *a_widget_ptr) {
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void desktop_controller_interface::insert(window *a_window_ptr) {
  if (!o_view_controller->m_viewport) {
    return;
  }

  o_view_controller->m_viewport->insert_window_to_view(a_window_ptr);
}

void desktop_controller_interface::on_ready() {
  if (o_view_controller->m_data_source) {
    Q_EMIT data(o_view_controller->m_data_source.data());
  }
}
}
