#include <view_controller.h>
#include <extensionmanager.h>
#include <QDebug>

#include <datasync.h>
#include <disksyncengine.h>
#include <session_sync.h>

namespace CherryKit {

class ViewController::PrivateViewControllerPlugin {
public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}

  QSharedPointer<DataSource> m_data_source;
  Space *m_viewport;
  QString m_name;

  std::vector<SessionSync *> m_session_list;
};

ViewController::ViewController(QObject *parent)
    : QObject(parent), o_view_controller(new PrivateViewControllerPlugin) {
  o_view_controller->m_viewport = 0;
}

void ViewController::revoke_previous_session(
    const std::string &a_session_object_name,
    std::function<void(ViewController *, SessionSync *)> a_callback) {
  ck::data_sync *sync =
      new ck::data_sync(session_database_name(a_session_object_name));
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->on_object_found([&](ck::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      QVariantMap session_data;

      ck::CkStringList prop_list = a_object.property_list();

      std::for_each(std::begin(prop_list), std::end(prop_list),
                    [&](const std::string &a_prop) {
        session_data[a_prop.c_str()] = a_object.property(a_prop).c_str();
      });

      start_session(
          a_session_object_name, session_data, true,
          [&](ViewController *a_controller, CherryKit::SessionSync *a_session) {
            a_callback(a_controller, a_session);
          });
    }
  });

  sync->find(a_session_object_name, "", "");
  delete sync;
}

void ViewController::write_session_data(const std::string &a_session_name) {
  std::string session_name = session_database_name(a_session_name);
  std::string key_name = a_session_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
  key_name += "_id";

  std::for_each(std::begin(o_view_controller->m_session_list),
                std::end(o_view_controller->m_session_list),
                [&](CherryKit::SessionSync *session_ref) {
    if (session_ref->is_purged())
      return;

    if (session_ref->session_group_key().compare(key_name) != 0)
      return;

    ck::data_sync *sync = new ck::data_sync(session_name);
    ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();
    sync->set_sync_engine(engine);

    session_ref->update_session();
    ck::SyncObject clock_session_obj;

    clock_session_obj.set_name(a_session_name);
    Q_FOREACH(const QString & a_key, session_ref->session_keys()) {
      clock_session_obj.set_property(
          a_key.toStdString(),
          std::string(session_ref->session_data(a_key).toByteArray()));
    }

    sync->on_object_found([&](ck::SyncObject &a_object,
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

int ViewController::session_count() {
  return o_view_controller->m_session_list.size();
}

ViewController::~ViewController() { delete o_view_controller; }

void ViewController::set_viewport(Space *a_view_ptr) {
  o_view_controller->m_viewport = a_view_ptr;
}

Space *ViewController::viewport() const {
  return o_view_controller->m_viewport;
}

void ViewController::start_session(
    const std::string &a_session_name, const QVariantMap &a_data,
    bool a_restore,
    std::function<void(ViewController *, SessionSync *)> a_callback) {
  CherryKit::SessionSync *session_ref =
      new CherryKit::SessionSync(a_session_name, a_data);
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

std::string
ViewController::session_database_name(const std::string &a_session_name) const {
  std::string key_name = a_session_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

  std::string session_db_name =
      o_view_controller->m_viewport->session_name_for_controller(
                                         controller_name()).toStdString() +
      "_org." + key_name + ".data";
  return session_db_name;
}

ActionList ViewController::actions() const { return ActionList(); }

void ViewController::request_action(const QString & /*actionName*/,
                                    const QVariantMap & /*args*/) {}

void ViewController::handle_drop_event(Widget * /*widget*/,
                                       QDropEvent * /*event*/) {}

DataSource *ViewController::dataSource() {
  return o_view_controller->m_data_source.data();
}

void ViewController::set_controller_name(const QString &a_name) {
  o_view_controller->m_name = a_name;
}

QString ViewController::controller_name() const {
  return o_view_controller->m_name;
}

QString ViewController::label() const { return QString(); }

void ViewController::configure(const QPointF &a_pos) { Q_UNUSED(a_pos) }

void ViewController::prepare_removal() {
  o_view_controller->m_data_source.clear();
}

bool ViewController::connect_to_data_source(const QString &a_source) {
  o_view_controller->m_data_source =
      ExtensionManager::instance()->data_engine(a_source);

  if (!o_view_controller->m_data_source.data()) {
    return 0;
  }

  connect(o_view_controller->m_data_source.data(), SIGNAL(ready()), this,
          SLOT(on_ready()));

  return true;
}

bool ViewController::remove_widget(Widget *a_widget_ptr) {
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void ViewController::insert(Window *a_window_ptr) {
  if (!o_view_controller->m_viewport) {
    return;
  }

  o_view_controller->m_viewport->insert_window_to_view(a_window_ptr);
}

void ViewController::on_ready() {
  if (o_view_controller->m_data_source) {
    Q_EMIT data(o_view_controller->m_data_source.data());
  }
}
}
