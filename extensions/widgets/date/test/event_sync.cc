#include <QApplication>
#include <QDebug>
#include <ck_image_io.h>

#include <ck_sync_object.h>
#include <ck_disk_engine.h>
#include <ck_data_sync.h>

#include <QImage>
#include <iostream>
#include <thread>

#ifdef Q_OS_WIN
#include <memory>
#endif

#include "../event_store.h"

#include "px_bench.h"

#define CK_ASSERT(condition, message)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__         \
                << " line " << __LINE__ << ": " << message << std::endl;       \
      std::exit(EXIT_FAILURE);                                                 \
    } else {                                                                   \
      std::cout << __LINE__ << " " << __func__ << "  " << #condition           \
                << "  [PASS]" << std::endl;                                    \
    }                                                                          \
  } while (false)

using namespace cherry_kit;

void process_event()
{
    std::cout << __FUNCTION__ << " :wait for response" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << __FUNCTION__ << " :thread exit" << std::endl;
}

void insert_event() {
  std::unique_ptr<event_store> store (new event_store("default"));

  process_event();
}

int main(int argc, char *argv[]) {
  //QCoreApplication app(argc, argv);

  insert_event();
  return EXIT_SUCCESS;
}
