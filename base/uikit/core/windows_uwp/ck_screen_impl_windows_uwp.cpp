#include "ck_screen_impl_windows_uwp.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QRectF>
#include <QDebug>
#include <QScreen>

#include <functional>
#include <wrl.h>
#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.ui.xaml.h>
#include <windows.ui.xaml.controls.h>
#include <windows.ui.viewmanagement.h>
#include <windows.graphics.display.h>
#include <windows.ui.core.h>

using namespace ABI::Windows::UI::ViewManagement;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;

class windows_core_window {
public:
	windows_core_window() {
      CoreWindow ^core_window = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;

	  if (core_window) {
		  m_priv_dispatcher = core_window->Dispatcher;
		  m_priv_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([]() {
	        auto _display_info = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
			auto _application_view =  Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();

			Windows::Foundation::Rect bounds = _application_view->VisibleBounds;
			double scale_factor = _display_info->RawPixelsPerViewPixel; 
			qDebug() << "Current Display Resolution (X) : " << (float) (scale_factor * bounds.Width);
			qDebug() << "Current Display Resolution (Y) : " << (float) (scale_factor * bounds.Height);
		  }));
	  }
	  else {
	  }
	}
	virtual ~windows_core_window() {}

	CoreDispatcher ^m_priv_dispatcher{ nullptr };
};

cherry_kit::screen::platform_screen::platform_screen() {
	windows_core_window();
}

cherry_kit::screen::platform_screen::~platform_screen() {
}

int cherry_kit::screen::platform_screen::screen_count() const {
  return qApp->desktop()->screenCount();
}

float cherry_kit::screen::platform_screen::scale_factor(int a_id) const {
  float desktop_scale =
      display_width(a_id) / desktop_width(a_id);

  if (desktop_scale < 1) {
    return 1.0f / desktop_scale;
  }

  return desktop_scale;
}

float cherry_kit::screen::platform_screen::desktop_width(int a_id) const {
  return 1920.0f;
}

float cherry_kit::screen::platform_screen::desktop_height(int a_id) const {
  float rv = 1080.0f;

  rv = (display_height(a_id) / display_width(a_id)) *
       desktop_width(a_id);

  return rv;
}

float cherry_kit::screen::platform_screen::display_width(int a_id) const {
  return QApplication::primaryScreen()->size().width();
}

float cherry_kit::screen::platform_screen::display_height(int a_id) const {
  return QApplication::primaryScreen()->size().height();
}
