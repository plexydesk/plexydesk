#ifndef CONTROLLER_ACTION_H
#define CONTROLLER_ACTION_H

#include <cstring>
#include <string>

namespace UI {

class ControllerAction {
public:
  ControllerAction();

  ~ControllerAction();

  virtual std::string name() const;

  virtual void setName(const std::string &name);

  virtual unsigned int id() const;

  virtual void setId(unsigned int id);

  virtual void setVisibility(bool visibility = true);

  virtual bool isVisibile() const;

  virtual void setIcon(const std::string icon);

  virtual std::string icon() const;

private:
  class PrivateControllerAction;
  PrivateControllerAction *const d;
};
}

#endif // CONTROLLER_ACTION_H
