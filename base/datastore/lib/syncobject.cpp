#include "syncobject.h"

#include <QDebug>

#include <datasync.h>

#include <chrono>

namespace ck {

class SyncObject::PrivateSyncObject {
public:
  PrivateSyncObject() : m_sync_store(0) {}
  ~PrivateSyncObject() {
    std::for_each(std::begin(m_child_map), std::end(m_child_map),
                  [](std::pair<uint, SyncObject *> record) {
      if (record.second)
        delete record.second;
    });

    m_child_map.clear();
    m_property_dict.clear();
  }

  uint m_time_stamp;
  // QMap<QString, QVariant> m_property_dict;
  std::map<std::string, std::string> m_property_dict;
  std::map<uint, SyncObject *> m_child_map;
  SyncObject *m_parent;
  uint m_child_count;

  std::string m_object_name;
  uint m_object_key;

  data_sync *m_sync_store;
};

void SyncObject::update_time_stamp() {
  // todo: Add the creation time from System::Now
  p_object->m_time_stamp = 0;
}

void SyncObject::removeObject(uint key) {
  // todo;
}

SyncObject::SyncObject(SyncObject *parent) : p_object(new PrivateSyncObject) {
  update_time_stamp();
  p_object->m_object_key = -1;
  p_object->m_child_count = -1;
  p_object->m_parent = 0;

  if (parent)
    parent->add_child(this);
}

SyncObject::~SyncObject() {
  qDebug() << Q_FUNC_INFO;
  delete p_object;
}

uint SyncObject::time_stamp() const { return p_object->m_time_stamp; }

void SyncObject::set_time_stamp(uint timestamp) {
  p_object->m_time_stamp = timestamp;
}

unsigned int SyncObject::update_time_stamp() const {}

void SyncObject::set_name(const std::string &name) {
  p_object->m_object_name = name;
}

std::string SyncObject::name() const { return p_object->m_object_name; }

void SyncObject::set_key(uint key) { p_object->m_object_key = key; }

uint SyncObject::key() const { return p_object->m_object_key; }

SyncObject *SyncObject::parent() const { return p_object->m_parent; }

void SyncObject::set_parent(SyncObject *parent) { p_object->m_parent = parent; }

void SyncObject::set_property(const std::string &name,
                              const std::string &value) {
  p_object->m_property_dict[name] = value;
  update_time_stamp();
}

void SyncObject::set_data_sync(data_sync *a_sync) {
  if (p_object->m_sync_store)
    return;

  p_object->m_sync_store = a_sync;
}

void SyncObject::sync() {
  if (p_object->m_sync_store) {
    qDebug() << Q_FUNC_INFO << "Saved";
    p_object->m_sync_store->save_object(*this);
  }
}

CkStringList SyncObject::property_list() const {
  CkStringList rv;
  for (auto handle : p_object->m_property_dict) {
    rv.push_back(handle.first);
  }
  return rv;
}

bool SyncObject::has_property(const std::string &a_property) const {
  if (p_object->m_property_dict.find(a_property) == p_object->m_property_dict.end())
    return 0;

  return 1;
}

std::string SyncObject::property(const std::string &name) const {
  if (!has_property(name))
    return std::string();

  return p_object->m_property_dict.at(name);
}

CkObjectList SyncObject::child_objects() const {
  CkObjectList rv;
  for (auto handle : p_object->m_child_map) {
    rv.push_back(handle.second);
  }

  return rv;
}

SyncObject *SyncObject::childObject(uint key) {
  if (p_object->m_child_map.find(key) != p_object->m_child_map.end()) {
    return p_object->m_child_map[key];
  }

  return nullptr;
}

SyncObject *SyncObject::create_new(const std::string &name) {
  SyncObject *rv = new SyncObject(this);
  rv->set_name(name);
  p_object->m_child_count = p_object->m_child_count + 1;
  rv->set_key(p_object->m_child_count);
  rv->set_parent(this);
  p_object->m_child_map[p_object->m_child_count] = rv;
  return rv;
}

std::string SyncObject::dump_content() const { return ""; }

bool SyncObject::contains(SyncObject *object) {
  if (!this->has_children()) {
    qDebug() << Q_FUNC_INFO
             << "Error: No Children for Object : " << object->name().c_str();
    return false;
  }

  for (std::map<uint, SyncObject *>::iterator it =
           p_object->m_child_map.begin();
       it != p_object->m_child_map.end(); ++it) {

    if (!it->second)
      continue;

    if (it->second->is_similar(object)) {
      return true;
    }
  }

  qDebug() << Q_FUNC_INFO
           << "No Object found for Object:" << object->name().c_str();
  return false;
}

bool SyncObject::is_similar(SyncObject *object) {
  if (object->name() != this->name()) {
    return false;
  }

  // check properties
  CkStringList ck_prop_list = object->property_list();

  for (int p = 0; p < ck_prop_list.size(); p++) {
    std::string prop = ck_prop_list.at(p);

    if (prop == "timestamp" || prop == "key") {
      continue;
    }

    if (p_object->m_property_dict.find(prop) ==
        p_object->m_property_dict.end()) {
      return false;
    }

    if (this->property(prop).compare(object->property(prop)) != 0) {
      return false;
    }
  }

  return true;
}

void SyncObject::replace(SyncObject *object) {}

SyncObject *SyncObject::childObject(const std::string &name) {
  if (!this->has_children()) {
    SyncObject *rv = create_new(name);
    return rv;
  }

  for (std::map<uint, SyncObject *>::iterator it =
           p_object->m_child_map.begin();
       it != p_object->m_child_map.end(); ++it) {

    if (!it->second)
      continue;

    if (it->second->name() == name) {
      return it->second;
    }
  }

  return 0;
}

void SyncObject::linksToObject(const std::string &dataStoreName,
                               const std::string &objectName) {}

bool SyncObject::has_children() const {
  if (child_count() > 0) {
    return true;
  }
  return false;
}

uint SyncObject::child_count() const {
  unsigned int count = 0;

  for (std::map<uint, SyncObject *>::iterator it =
           p_object->m_child_map.begin();
       it != p_object->m_child_map.end(); ++it) {
    count++;
  }

  return count;
}

void SyncObject::add_child(SyncObject *object) {
  p_object->m_child_count += 1;
  uint currentKey = p_object->m_child_count;

  object->set_parent(this);
  p_object->m_child_map[currentKey] = object;
  update_time_stamp();
}
}
