/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "ck_sync_object.h"

#include <QDebug>

#include <ck_data_sync.h>

#include <chrono>

namespace cherry_kit {

class sync_object::Privatesync_object {
public:
  Privatesync_object() : m_sync_store(0) {}
  ~Privatesync_object() {
    std::for_each(std::begin(m_child_map), std::end(m_child_map),
                  [](std::pair<uint, sync_object *> record) {
      if (record.second)
        delete record.second;
    });

    m_child_map.clear();
    m_property_dict.clear();
  }

  uint m_time_stamp;
  // QMap<QString, QVariant> m_property_dict;
  std::map<std::string, std::string> m_property_dict;
  std::map<uint, sync_object *> m_child_map;
  sync_object *m_parent;
  uint m_child_count;

  std::string m_object_name;
  uint m_object_key;

  data_sync *m_sync_store;
};

void sync_object::update_time_stamp() {
  // todo: Add the creation time from System::Now
  p_object->m_time_stamp = 0;
}

void sync_object::remove_object(uint key) {
  std::map<uint, sync_object *>::iterator ck_it =
      p_object->m_child_map.find(key);

  if (ck_it == p_object->m_child_map.end())
    return;

  delete p_object->m_child_map.at(key);

  p_object->m_child_map.erase(ck_it);
  p_object->m_child_count -= 1;
}

sync_object::sync_object(sync_object *parent)
    : p_object(new Privatesync_object) {
  update_time_stamp();
  p_object->m_object_key = -1;
  p_object->m_child_count = -1;
  p_object->m_parent = 0;

  if (parent)
    parent->add_child(this);
}

sync_object::~sync_object() { delete p_object; }

unsigned int sync_object::time_stamp() const { return p_object->m_time_stamp; }

void sync_object::set_time_stamp(unsigned int timestamp) {
  p_object->m_time_stamp = timestamp;
}

unsigned int sync_object::update_time_stamp() const {}

void sync_object::set_name(const std::string &name) {
  p_object->m_object_name = name;
}

std::string sync_object::name() const { return p_object->m_object_name; }

void sync_object::set_key(unsigned int key) { p_object->m_object_key = key; }

uint sync_object::key() const { return p_object->m_object_key; }

sync_object *sync_object::parent() const { return p_object->m_parent; }

void sync_object::set_parent(sync_object *parent) {
  p_object->m_parent = parent;
}

void sync_object::set_property(const std::string &name,
                               const std::string &value) {
  p_object->m_property_dict[name] = value;
  update_time_stamp();
}

void sync_object::set_data_sync(data_sync *a_sync) {
  if (p_object->m_sync_store)
    return;

  p_object->m_sync_store = a_sync;
}

void sync_object::sync() {
  if (p_object->m_sync_store) {
    qDebug() << Q_FUNC_INFO << "Saved";
    p_object->m_sync_store->save_object(*this);
  }
}

ck_string_list sync_object::property_list() const {
  ck_string_list rv;
  for (auto handle : p_object->m_property_dict) {
    rv.push_back(handle.first);
  }
  return rv;
}

bool sync_object::has_property(const std::string &a_property) const {
  if (p_object->m_property_dict.find(a_property) ==
      p_object->m_property_dict.end())
    return 0;

  return 1;
}

std::string sync_object::property(const std::string &name) const {
  if (!has_property(name))
    return std::string();

  return p_object->m_property_dict.at(name);
}

sync_object_list sync_object::child_objects() const {
  sync_object_list rv;
  for (auto handle : p_object->m_child_map) {
    rv.push_back(handle.second);
  }

  return rv;
}

sync_object *sync_object::childObject(unsigned int key) {
  if (p_object->m_child_map.find(key) != p_object->m_child_map.end()) {
    return p_object->m_child_map[key];
  }

  return nullptr;
}

sync_object *sync_object::create_new(const std::string &name) {
  sync_object *rv = new sync_object(this);
  rv->set_name(name);
  p_object->m_child_count = p_object->m_child_count + 1;
  rv->set_key(p_object->m_child_count);
  rv->set_parent(this);
  p_object->m_child_map[p_object->m_child_count] = rv;
  return rv;
}

std::string sync_object::dump_content() const { return ""; }

bool sync_object::contains(sync_object *object) {
  if (!this->has_children()) {
    qDebug() << Q_FUNC_INFO
             << "Error: No Children for Object : " << object->name().c_str();
    return false;
  }

  for (std::map<uint, sync_object *>::iterator it =
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

bool sync_object::is_similar(sync_object *object) {
  if (object->name() != this->name()) {
    return false;
  }

  // check properties
  ck_string_list ck_prop_list = object->property_list();

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

void sync_object::replace(sync_object *object) {}

sync_object *sync_object::childObject(const std::string &name) {
  if (!this->has_children()) {
    sync_object *rv = create_new(name);
    return rv;
  }

  for (std::map<uint, sync_object *>::iterator it =
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

void sync_object::linksToObject(const std::string &dataStoreName,
                                const std::string &objectName) {}

bool sync_object::has_children() const {
  if (child_count() > 0) {
    return true;
  }
  return false;
}

unsigned int sync_object::child_count() const {
  unsigned int count = 0;

  for (std::map<uint, sync_object *>::iterator it =
           p_object->m_child_map.begin();
       it != p_object->m_child_map.end(); ++it) {
    count++;
  }

  return count;
}

void sync_object::add_child(sync_object *object) {
  p_object->m_child_count += 1;
  uint currentKey = p_object->m_child_count;

  object->set_parent(this);
  p_object->m_child_map[currentKey] = object;
  update_time_stamp();
}
}
