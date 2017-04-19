#include "ck_timer.h"

#include <thread>

#include <QDebug>
#include <QTimer>

#include <functional>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace cherry_kit {

#ifdef __APPLE__

typedef std::function<void()> timeout_task_t;
#endif

class timer::private_timer {
public:
    private_timer() : m_is_running(0) {}
    ~private_timer() {
      m_is_running = 0;
#if defined(__QT4_TOOLKIT__) && defined (__APPLE__)
      cf_stop_timer(m_current_timer);
#endif
    }

    std::function<void()> m_func;
    bool m_is_running;
    double m_duration;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    QTimer m_qt_timer;

#if defined(__QT4_TOOLKIT__) && defined (__APPLE__)
    CFRunLoopTimerRef m_current_timer;

    void exec() {
      if (m_func)
        m_func();
    }

    static void cf_timeout_callback(CFRunLoopTimerRef a_timer, void *ctx) {
        timer::private_timer *_priv = (timer::private_timer*) ctx;

        if (!_priv)
          return;

        _priv->exec();
    }

    CFRunLoopTimerRef cf_init_timer(double a_timeout, timeout_task_t a_task) {
        CFRunLoopTimerContext _ctx= { 0, this, NULL, NULL, NULL };
        CFRunLoopTimerRef _timer=  CFRunLoopTimerCreate(kCFAllocatorDefault,
                                                        CFAbsoluteTimeGetCurrent(),
                                                        a_timeout,
                                                        0, 0, &timer::private_timer::cf_timeout_callback, &_ctx);

        CFRunLoopAddTimer(CFRunLoopGetCurrent(),
                          _timer, kCFRunLoopCommonModes);

        return _timer;
    }

    void cf_stop_timer(CFRunLoopTimerRef a_timer) {
      if (!cf_is_valid(a_timer))
        return;

      CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), a_timer, kCFRunLoopCommonModes);
      CFRunLoopTimerInvalidate(a_timer);

      CFRelease(a_timer);

      m_current_timer = NULL;
    }

    bool cf_is_valid(CFRunLoopTimerRef a_timer) {
      if (!a_timer)
        return false;

      return (bool) CFRunLoopTimerIsValid(a_timer);
    }

    bool cf_is_active(CFRunLoopTimerRef a_timer) {
      if (!a_timer)
        return false;

      return (bool) CFRunLoopTimerDoesRepeat(a_timer);
    }
#endif
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

#if defined(__QT4_TOOLKIT__) && defined (__APPLE__)
  priv->m_current_timer = NULL;
#endif
}

timer::~timer() { delete priv; }

void timer::start() const {
  priv->m_start_time = std::chrono::high_resolution_clock::now();
#ifdef __QT5_TOOLKIT__
  priv->m_qt_timer.start(priv->m_duration);
#endif

#if defined(__QT4_TOOLKIT__) && defined (__APPLE__)
  if (priv->cf_is_valid(priv->m_current_timer) &&
      priv->cf_is_valid(priv->m_current_timer)) {
      priv->cf_stop_timer(priv->m_current_timer);
    }

    priv->m_current_timer = priv->cf_init_timer(priv->m_duration / 1000.0, priv->m_func);
#endif

}

void timer::stop() {
  priv->m_is_running = 0;
  priv->m_qt_timer.stop();

#if defined(__QT4_TOOLKIT__) && defined (__APPLE__)
  if (priv->cf_is_valid(priv->m_current_timer) &&
      priv->cf_is_valid(priv->m_current_timer)) {
      priv->cf_stop_timer(priv->m_current_timer);
    }

#endif
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
