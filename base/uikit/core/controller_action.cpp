#include "controller_action.h"

namespace CherryKit {

class ControllerAction::PrivateControllerAction {
public:
  PrivateControllerAction() : m_action_visibility(true), m_action_id(0) {}

  ~PrivateControllerAction() {}

  unsigned int m_action_id;
  std::string m_action_name;
  std::string m_action_icon;
  bool m_action_visibility;
};

ControllerAction::ControllerAction() : o_controller_action(new PrivateControllerAction) {}

CherryKit::ControllerAction::~ControllerAction() { delete o_controller_action; }

std::string ControllerAction::name() const { return o_controller_action->m_action_name; }

void ControllerAction::setName(const std::string &a_name) {
  o_controller_action->m_action_name.clear();
  o_controller_action->m_action_name = a_name;
}

unsigned int ControllerAction::id() const { return o_controller_action->m_action_id; }

void ControllerAction::setId(unsigned int a_id) { o_controller_action->m_action_id = a_id; }

void ControllerAction::setVisibility(bool a_visibility) {
  o_controller_action->m_action_visibility = a_visibility;
}

bool ControllerAction::isVisibile() const { return o_controller_action->m_action_visibility; }

void ControllerAction::setIcon(const std::string a_icon) {
  o_controller_action->m_action_icon.clear();
  o_controller_action->m_action_icon = a_icon;
}

std::string ControllerAction::icon() const { return o_controller_action->m_action_icon; }
}
