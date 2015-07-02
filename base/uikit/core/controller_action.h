#ifndef CONTROLLER_ACTION_H
#define CONTROLLER_ACTION_H

#include <cstring>
#include <string>

namespace CherryKit {

class ControllerAction {
public:
  ControllerAction();

  ~ControllerAction();

  virtual std::string name() const;

  virtual void setName(const std::string &a_name);

  virtual unsigned int id() const;

  virtual void setId(unsigned int a_id);

  virtual void setVisibility(bool a_visibility = true);

  virtual bool isVisibile() const;

  virtual void setIcon(const std::string a_icon);

  virtual std::string icon() const;

private:
  class PrivateControllerAction;
  PrivateControllerAction *const o_controller_action;
};
}

#endif // CONTROLLER_ACTION_H
