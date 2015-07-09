/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#ifndef SYNCOBJECT_H
#define SYNCOBJECT_H

#include <QuetzalDataKit_export.h>
#include <vector>
#include <string>
#include <algorithm>

namespace cherry {

class sync_object;

typedef std::vector<std::string> CkStringList;
typedef std::vector<sync_object *> CkObjectList;

class data_sync;
/**
    * @brief
    *
    */
class QuetzalDataKit_EXPORT sync_object {

public:
  /**
      * @brief Creates a new Object
      *
      * @param parent the pointer to the parent object
      */
  explicit sync_object(sync_object *a_parent_ptr = 0);

  /**
      * @brief
      *
      */
  virtual ~sync_object();

  /**
      * @brief
      *
      * @return uint
      */
  virtual uint time_stamp() const;

  /**
      * @brief
      *
      * @param timestamp
      */
  virtual void set_time_stamp(uint timestamp);

  /**
      * @brief Time stamp data of the object.
      *
      * @return uint
      */
  virtual unsigned int update_time_stamp() const;

  /**
      * @brief
      *
      * @param name
      */
  void set_name(const std::string &name);

  /**
      * @brief
      *
      * @return QString
      */
  std::string name() const;

  void set_key(uint key);
  uint key() const;

  sync_object *parent() const;
  void set_parent(sync_object *a_parent_ptr);

  void set_property(const std::string &name, const std::string &property);
  std::string property(const std::string &name) const;
  CkStringList property_list() const;
  bool has_property(const std::string &a_property) const;

  bool has_children() const;
  uint child_count() const;
  void add_child(sync_object *object);

  CkObjectList child_objects() const;
  sync_object *childObject(uint key);
  sync_object *childObject(const std::string &name);
  void linksToObject(const std::string &dataStoreName,
                     const std::string &objectName);

  void update_time_stamp();

  void removeObject(uint key);

  void set_data_sync(data_sync *a_sync);
  virtual void sync();

  // protected:
  sync_object *create_new(const std::string &name);
  std::string dump_content() const;
  bool contains(sync_object *object);
  bool is_similar(sync_object *object);
  void replace(sync_object *object);

private:
  class Privatesync_object;
  Privatesync_object *const p_object; /**< TODO */
};
}
#endif // SYNCOBJECT_H
