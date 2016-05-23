#include "ck_ui_action.h"
#include <QDebug>

namespace cherry_kit {

class ui_action::PrivateControllerAction {
public:
  PrivateControllerAction() : m_action_visibility(0), m_action_id(0) {}
  ~PrivateControllerAction() {}

  unsigned int m_action_id;

  std::string m_action_name;
  std::string m_action_icon;
  std::string m_controller_name;

  bool m_action_visibility;

  ui_action_list m_child_action_list;

  std::vector<ui_task_callback_t> m_callback_list;
};

ui_action::ui_action() : priv(new PrivateControllerAction) {}

ui_action::ui_action(const ui_action &copy)
    : priv(new PrivateControllerAction) {
  priv->m_action_icon = copy.priv->m_action_icon;
  priv->m_action_name = copy.priv->m_action_name;
  priv->m_controller_name = copy.priv->m_controller_name;
  priv->m_action_id = copy.priv->m_action_id;
  priv->m_action_visibility = copy.priv->m_action_visibility;
  std::for_each(std::begin(copy.priv->m_child_action_list),
                std::end(copy.priv->m_child_action_list),
                [this](ui_action action) {
    priv->m_child_action_list.push_back(action);
  });

  std::for_each(std::begin(copy.priv->m_callback_list),
                std::end(copy.priv->m_callback_list),
                [&](ui_task_callback_t a_func) {
    priv->m_callback_list.push_back(a_func);
  });
}

cherry_kit::ui_action::~ui_action() { delete priv; }

std::string ui_action::name() const { return priv->m_action_name; }

void ui_action::set_name(const std::string &a_name) {
  priv->m_action_name.clear();
  priv->m_action_name = a_name;
}

std::string ui_action::controller() const { return priv->m_controller_name; }

void ui_action::set_controller(const std::string &a_name) {
  priv->m_controller_name = a_name;
}

unsigned int ui_action::id() const { return priv->m_action_id; }

void ui_action::set_id(unsigned int a_id) { priv->m_action_id = a_id; }

void ui_action::set_visible(bool a_visibility) {
  priv->m_action_visibility = a_visibility;
}

bool ui_action::is_visibile() const { return priv->m_action_visibility; }

void ui_action::set_icon(const std::string a_icon) {
  priv->m_action_icon.clear();
  priv->m_action_icon = a_icon;
}

std::string ui_action::icon() const { return priv->m_action_icon; }

void ui_action::add_action(const ui_action &action) {
  priv->m_child_action_list.push_back(action);
}

ui_action_list ui_action::sub_actions() const {
  return priv->m_child_action_list;
}

void ui_action::set_task(ui_task_callback_t callback) {
  priv->m_callback_list.push_back(callback);
}

void ui_action::execute(const ui_task_data_t &a_data) const {
  std::for_each(std::begin(priv->m_callback_list),
                std::end(priv->m_callback_list),
                [&](ui_task_callback_t a_func) {

    if (a_func)
      a_func(this, a_data);
  });
}

void ui_action::execute(const std::string &a_task_name,
                        const ui_task_data_t &a_data) {
  if (priv->m_action_name.compare(a_task_name) == 0) {
    execute(a_data);
    return;
  }

  std::for_each(std::begin(priv->m_child_action_list),
                std::end(priv->m_child_action_list), [&](ui_action &a_action) {
    a_action.execute(a_task_name, a_data);
  });
}
}
