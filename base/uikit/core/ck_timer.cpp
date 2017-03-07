#include "ck_timer.h"

#include <thread>

#include <QDebug>
#include <QTimer>

namespace cherry_kit {
class timer::private_timer {
public:
  private_timer() : m_is_running(0) {}
  ~private_timer() { m_is_running = 0; }

  std::function<void()> m_func;
  bool m_is_running;
  int m_duration;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
  QTimer m_qt_timer;
};

timer::timer(int a_duration) : priv(new private_timer) {
  priv->m_duration = a_duration;
  priv->m_is_running = 1;

#ifdef __QT5_TOOLKIT__
  priv->m_qt_timer.connect(&priv->m_qt_timer, &QTimer::timeout, [=]() {
    if (priv->m_func)
      priv->m_func();
  });
#endif
}

timer::~timer() { delete priv; }

void timer::start() const {
  priv->m_start_time = std::chrono::high_resolution_clock::now();
  priv->m_qt_timer.start(priv->m_duration);
}

void timer::stop() {
  priv->m_is_running = 0;
  priv->m_qt_timer.stop();
}

int timer::elapsed_seconds() {
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time =
      std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::seconds>(
      end_time - priv->m_start_time).count();
}

int timer::elapsed_minutes() {
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time =
      std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::minutes>(
              end_time - priv->m_start_time).count();
}

bool timer::is_active() const {
  return priv->m_qt_timer.isActive();
}

void timer::start_once(int a_duration, std::function<void()> a_callback) {
  std::thread timeout_thread([=]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(a_duration));
    if (a_callback)
      a_callback();
  });

  timeout_thread.detach();
}

void timer::on_timeout(std::function<void()> a_callback) {
  priv->m_func = a_callback;
}
}
