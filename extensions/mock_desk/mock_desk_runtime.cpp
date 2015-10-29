#include "mock_desk_runtime.h"

#include <ck_extension_manager.h>
#include <ck_resource_manager.h>
#include <config.h>

Runtime::Runtime(QObject *parent) : QObject(parent) {}

Runtime::~Runtime() {
  if (m_manager)
    delete m_manager;
}

void Runtime::open_window() {
  cherry_kit::extension_manager *loader = cherry_kit::extension_manager::init(
      QDir::toNativeSeparators(
          QLatin1String("/Users/siraj/Applications/release/plexydesk/") +
          QLatin1String("/plexydesk.app/")+
          QLatin1String("/Contents/PlugIns/plexydesk/data/")),
      QDir::toNativeSeparators(QLatin1String("/Users/siraj/Applications/release/plexydesk/") +
                               QLatin1String("/plexydesk.app/") +
                               QLatin1String("/Contents/PlugIns/plexydesk/")));

  m_manager = new DesktopManager();
  m_manager->resize(640, 480);
  m_manager->add_sample_text();
  m_manager->show();
 /*
  m_manager->add_default_controller("classicbackdrop");
  m_manager->add_default_controller("dockwidget");
  m_manager->add_default_controller("datewidget");
  m_manager->add_default_space();
  m_manager->add_sample_text();
  m_manager->show();
  m_manager->set_accelerated_rendering(false);
  m_manager->move_to_screen(0);
  */
}

void Runtime::init() {
  m_manager = new DesktopManager();
  m_manager->add_default_controller("classicbackdrop");
  m_manager->add_default_controller("dockwidget");
  m_manager->add_default_controller("plexyclock");
  m_manager->add_default_controller("desktopnoteswidget");
  m_manager->add_default_controller("folderwidget");
  m_manager->add_default_controller("datewidget");
  m_manager->add_default_controller("photoframe");

  m_manager->add_default_space();
  m_manager->show();
}

void Runtime::run_remove_space_test() {
  qDebug() << Q_FUNC_INFO << "Remove Space";
  m_manager->remove(m_manager->current_active_space());
}

void Runtime::end() {
  delete m_manager;
  m_manager = 0;

  cherry_kit::resource_manager *rm = cherry_kit::resource_manager::instance();

  delete rm;

  cherry_kit::extension_manager::instance()->destroy_instance();
  qApp->quit();
}
