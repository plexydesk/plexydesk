#ifndef SYNCOBJECT_H
#define SYNCOBJECT_H

#include <QuetzalDataKit_export.h>
#include <vector>
#include <string>
#include <algorithm>

namespace ck {

class SyncObject;

typedef std::vector<std::string> CkStringList;
typedef std::vector<SyncObject *> CkObjectList;

class data_sync;
/**
    * @brief
    *
    */
class QuetzalDataKit_EXPORT SyncObject {

public:
  /**
      * @brief Creates a new Object
      *
      * @param parent the pointer to the parent object
      */
  explicit SyncObject(SyncObject *a_parent_ptr = 0);

  /**
      * @brief
      *
      */
  virtual ~SyncObject();

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

  SyncObject *parent() const;
  void set_parent(SyncObject *a_parent_ptr);

  void set_property(const std::string &name, const std::string &property);
  std::string property(const std::string &name) const;
  CkStringList property_list() const;
  bool has_property(const std::string &a_property) const;

  bool has_children() const;
  uint child_count() const;
  void add_child(SyncObject *object);

  CkObjectList child_objects() const;
  SyncObject *childObject(uint key);
  SyncObject *childObject(const std::string &name);
  void linksToObject(const std::string &dataStoreName,
                     const std::string &objectName);

  void update_time_stamp();

  void removeObject(uint key);

  void set_data_sync(data_sync *a_sync);
  virtual void sync();

  // protected:
  SyncObject *create_new(const std::string &name);
  std::string dump_content() const;
  bool contains(SyncObject *object);
  bool is_similar(SyncObject *object);
  void replace(SyncObject *object);

private:
  class PrivateSyncObject;
  PrivateSyncObject *const p_object; /**< TODO */
};
}
#endif // SYNCOBJECT_H
