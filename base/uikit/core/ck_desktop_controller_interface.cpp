#include <ck_desktop_controller_interface.h>
#include <ck_extension_manager.h>
#include <QDebug>

#include <ck_data_sync.h>
#include <ck_disk_engine.h>
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
    : QObject(parent), priv(new PrivateViewControllerPlugin) {
  priv->m_viewport = 0;
}

void desktop_controller_interface::revoke_previous_session(
    const std::string &a_session_object_name,
    std::function<void(desktop_controller_interface *, session_sync *)>
        a_callback) {
  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync(session_store_name(a_session_object_name));
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();
  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      QVariantMap session_data;

      cherry_kit::ck_string_list prop_list = a_object.property_list();

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
  std::string session_name = session_store_name(a_session_name);
  std::string key_name = a_session_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
  key_name += "_id";

  std::for_each(std::begin(priv->m_session_list),
                std::end(priv->m_session_list),
                [&](cherry_kit::session_sync *session_ref) {
    if (session_ref->is_purged())
      return;

    if (session_ref->session_group_key().compare(key_name) != 0)
      return;

    cherry_kit::data_sync *sync = new cherry_kit::data_sync(session_name);
    cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();
    sync->set_sync_engine(engine);

    session_ref->update_session();
    cherry_kit::sync_object clock_session_obj;

    clock_session_obj.set_name(a_session_name);
    Q_FOREACH(const QString & a_key, session_ref->session_keys()) {
      clock_session_obj.set_property(
          a_key.toStdString(),
          std::string(session_ref->session_data(a_key).toByteArray()));
    }

    sync->on_object_found([&](cherry_kit::sync_object &a_object,
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
  return priv->m_session_list.size();
}

desktop_controller_interface::~desktop_controller_interface() {
  delete priv;
}

void desktop_controller_interface::set_viewport(space *a_view_ptr) {
  priv->m_viewport = a_view_ptr;
}

space *desktop_controller_interface::viewport() const {
  return priv->m_viewport;
}

void desktop_controller_interface::start_session(
    const std::string &a_session_name, const QVariantMap &a_data,
    bool a_restore, std::function<void(desktop_controller_interface *,
                                       session_sync *)> a_callback) {
  cherry_kit::session_sync *session_ref =
      new cherry_kit::session_sync(a_session_name, a_data);
  session_ref->set_session_id(
      a_data[QString::fromStdString(a_session_name).toLower() + "_id"].toInt());

  priv->m_session_list.push_back(session_ref);

  std::function<void()> on_session_callback =
      std::bind(a_callback, this, session_ref);

  session_ref->on_session_init(on_session_callback);

  session_ref->session_init();
  if (viewport() && a_restore == false) {
    viewport()->update_session_value(controller_name(), "", "");
  }
}

std::string desktop_controller_interface::session_store_name(
    const std::string &a_name) const {
  std::string key_name = a_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

  std::string session_db_name =
      priv->m_viewport->session_name_for_controller(
                                         controller_name()).toStdString() +
      "_org." + key_name + ".data";
  return session_db_name;
}

void desktop_controller_interface::handle_drop_event(widget * /*widget*/,
                                                     QDropEvent * /*event*/) {}

void desktop_controller_interface::set_controller_name(const QString &a_name) {
  priv->m_name = a_name;
}

QString desktop_controller_interface::controller_name() const {
  return priv->m_name;
}

void desktop_controller_interface::prepare_removal() {
  priv->m_data_source.clear();
}

bool desktop_controller_interface::remove_widget(widget *a_widget_ptr) {
  return false;
}

void desktop_controller_interface::insert(window *a_window_ptr) {
  if (!priv->m_viewport) {
    return;
  }

  priv->m_viewport->insert_window_to_view(a_window_ptr);
}
}
