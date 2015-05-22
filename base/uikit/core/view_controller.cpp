#include <view_controller.h>
#include <extensionmanager.h>
#include <QDebug>

#include <datasync.h>
#include <disksyncengine.h>
#include <session_sync.h>

namespace UIKit {

class ViewController::PrivateViewControllerPlugin {
 public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}

  QSharedPointer<DataSource> m_data_source;
  Space* m_viewport;
  QString m_name;

  std::vector<SessionSync*> m_session_list;
};

ViewController::ViewController(QObject* parent)
    : QObject(parent), d(new PrivateViewControllerPlugin) {
  d->m_viewport = 0;
}

void ViewController::revoke_previous_session(
    const std::string& a_session_object_name,
    std::function<void(ViewController*, SessionSync*)> a_callback) {
  QuetzalKit::DataSync* sync =
      new QuetzalKit::DataSync(session_database_name(a_session_object_name));
  QuetzalKit::DiskSyncEngine* engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject& a_object,
                            const std::string& a_app_name,
                            bool a_found) {
    if (a_found) {
      QVariantMap session_data;

      Q_FOREACH(const QString & a_key, a_object.attributes()) {
        session_data[a_key] = a_object.attributeValue(a_key);
      }

      start_session(
          a_session_object_name,
          session_data,
          true,
          [&](ViewController* a_controller, UIKit::SessionSync* a_session) {
            a_callback(a_controller, a_session);
          });
    }
  });

  sync->find(a_session_object_name, "", "");
  delete sync;
}

void ViewController::write_session_data(const std::string& a_session_name) {
  std::string session_name = session_database_name(a_session_name);
  std::string key_name = a_session_name;
  std::transform(key_name.begin(),
                 key_name.end(),
                 key_name.begin(),
                 ::tolower);
  key_name += "_id";

  std::for_each(std::begin(d->m_session_list),
                std::end(d->m_session_list),
                [&](UIKit::SessionSync* session_ref) {
    if (session_ref->is_purged())
      return;

    if (session_ref->session_group_key().compare(key_name) != 0)
        return;

    QuetzalKit::DataSync* sync = new QuetzalKit::DataSync(session_name);
    QuetzalKit::DiskSyncEngine* engine = new QuetzalKit::DiskSyncEngine();
    sync->set_sync_engine(engine);

    session_ref->update_session();
    QuetzalKit::SyncObject clock_session_obj;

    clock_session_obj.setName(QString::fromStdString(a_session_name));
    Q_FOREACH(const QString & a_key, session_ref->session_keys()) {
      clock_session_obj.setObjectAttribute(a_key,
                                           session_ref->session_data(a_key));
    }

    sync->on_object_found([&](QuetzalKit::SyncObject& a_object,
                              const std::string& a_app_name,
                              bool a_found) {
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
   return d->m_session_list.size();
}

ViewController::~ViewController() { delete d; }

void ViewController::set_viewport(Space* a_view_ptr) {
  d->m_viewport = a_view_ptr;
}

Space* ViewController::viewport() const { return d->m_viewport; }

void ViewController::start_session(
    const std::string& a_session_name,
    const QVariantMap& a_data,
    bool a_restore,
    std::function<void(ViewController*, SessionSync*)> a_callback) {
  UIKit::SessionSync* session_ref =
      new UIKit::SessionSync(a_session_name, a_data);
  session_ref->set_session_id(
      a_data[QString::fromStdString(a_session_name).toLower() + "_id"].toInt());

  d->m_session_list.push_back(session_ref);

  std::function<void()> on_session_callback =
      std::bind(a_callback, this, session_ref);

  session_ref->on_session_init(on_session_callback);

  session_ref->session_init();
  if (viewport() && a_restore == false) {
    viewport()->update_session_value(controller_name(), "", "");
  }
}

std::string ViewController::session_database_name(
    const std::string& a_session_name) const {
  std::string key_name = a_session_name;
  std::transform(key_name.begin(),
                 key_name.end(),
                 key_name.begin(),
                 ::tolower);

  std::string session_db_name =
      d->m_viewport->session_name_for_controller(controller_name())
          .toStdString() +
      "_org." + key_name + ".data";
  return session_db_name;
}

ActionList ViewController::actions() const { return ActionList(); }

void ViewController::request_action(const QString& /*actionName*/,
                                    const QVariantMap& /*args*/) {}

void ViewController::handle_drop_event(Widget* /*widget*/,
                                       QDropEvent* /*event*/) {}

DataSource* ViewController::dataSource() { return d->m_data_source.data(); }

void ViewController::set_controller_name(const QString& a_name) {
  d->m_name = a_name;
}

QString ViewController::controller_name() const { return d->m_name; }

QString ViewController::label() const { return QString(); }

void ViewController::configure(const QPointF& a_pos) { Q_UNUSED(a_pos) }

void ViewController::prepare_removal() { d->m_data_source.clear(); }

bool ViewController::connect_to_data_source(const QString& a_source) {
  d->m_data_source = ExtensionManager::instance()->data_engine(a_source);

  if (!d->m_data_source.data()) {
    return 0;
  }

  connect(d->m_data_source.data(), SIGNAL(ready()), this, SLOT(on_ready()));

  return true;
}

bool ViewController::remove_widget(Widget* a_widget_ptr) {
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void ViewController::insert(Window* a_window_ptr) {
  if (!d->m_viewport) {
    return;
  }

  d->m_viewport->insert_window_to_view(a_window_ptr);
}

void ViewController::on_ready() {
  if (d->m_data_source) {
    Q_EMIT data(d->m_data_source.data());
  }
}
}
