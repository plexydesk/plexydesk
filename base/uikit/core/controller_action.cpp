#include "controller_action.h"

namespace UIKit {

class ControllerAction::PrivateControllerAction {
public:
  PrivateControllerAction() : m_action_visibility(true), m_action_id(0) {}

  ~PrivateControllerAction() {}

  unsigned int m_action_id;
  std::string m_action_name;
  std::string m_action_icon;
  bool m_action_visibility;  
};

ControllerAction::ControllerAction() : d(new PrivateControllerAction) {}

UIKit::ControllerAction::~ControllerAction() { delete d; }

std::string ControllerAction::name() const { return d->m_action_name; }

void ControllerAction::setName(const std::string &a_name) {
  d->m_action_name.clear();
  d->m_action_name = a_name;
}

unsigned int ControllerAction::id() const { return d->m_action_id; }

void ControllerAction::setId(unsigned int a_id) { d->m_action_id = a_id; }

void ControllerAction::setVisibility(bool a_visibility) {
  d->m_action_visibility = a_visibility;
}

bool ControllerAction::isVisibile() const { return d->m_action_visibility; }

void ControllerAction::setIcon(const std::string a_icon) {
  d->m_action_icon.clear();
  d->m_action_icon = a_icon;
}

std::string ControllerAction::icon() const { return d->m_action_icon; }
}
