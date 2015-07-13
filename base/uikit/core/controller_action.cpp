#include "controller_action.h"

namespace cherry_kit {

class ui_action::PrivateControllerAction {
public:
  PrivateControllerAction() : m_action_visibility(true), m_action_id(0) {}

  ~PrivateControllerAction() {}

  unsigned int m_action_id;
  std::string m_action_name;
  std::string m_action_icon;
  bool m_action_visibility;
};

ui_action::ui_action() : o_ui_action(new PrivateControllerAction) {}

cherry_kit::ui_action::~ui_action() { delete o_ui_action; }

std::string ui_action::name() const { return o_ui_action->m_action_name; }

void ui_action::set_name(const std::string &a_name) {
  o_ui_action->m_action_name.clear();
  o_ui_action->m_action_name = a_name;
}

unsigned int ui_action::id() const { return o_ui_action->m_action_id; }

void ui_action::set_id(unsigned int a_id) { o_ui_action->m_action_id = a_id; }

void ui_action::set_visible(bool a_visibility) {
  o_ui_action->m_action_visibility = a_visibility;
}

bool ui_action::is_visibile() const { return o_ui_action->m_action_visibility; }

void ui_action::set_icon(const std::string a_icon) {
  o_ui_action->m_action_icon.clear();
  o_ui_action->m_action_icon = a_icon;
}

std::string ui_action::icon() const { return o_ui_action->m_action_icon; }
}
