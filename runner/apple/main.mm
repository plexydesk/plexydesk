/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#include <config.h>

#import <Cocoa/Cocoa.h>

//#include <QtGui>
//#include <QMacNativeWidget>

#include <QApplication>
#include <QtCore>
#include <QIcon>
#include <ck_extension_manager.h>

#include <ck_workspace.h>
#include <ck_space.h>
#include <ck_config.h>
#include "desktopmanager.h"

#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_4
#include <objc/objc.h>
#include <objc/message.h>
#endif

#if defined (__APPLE__) && defined(__QT4_TOOLKIT__)
#include <carbon/carbon.h>
#endif

bool on_dock_clicked_func(id self,SEL _cmd,...)
{
  Q_UNUSED(self)
  Q_UNUSED(_cmd)

  [[NSWorkspace sharedWorkspace] hideOtherApplications];
  return YES;
}

class Runtime {
public:
  Runtime() {
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_4
    id cls = objc_getClass("NSApplication");
    SEL sharedApplication = sel_registerName("sharedApplication");
    id appInst = objc_msgSend(cls,sharedApplication);

    if(appInst != NULL)
    {
        id delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        id delClass = objc_msgSend(delegate,  sel_registerName("class"));
        //const char* tst = class_getName(delClass->isa);
        bool classMethodFunc = class_addMethod(
                    (id)delClass,
                    sel_registerName(
                        "applicationShouldHandleReopen:hasVisibleWindows:"),
                    (IMP)on_dock_clicked_func,"B@:");

        if (!classMethodFunc) {
            NSLog(@"Failed to register with your mac");
        }
    }
#endif

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath =
            CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr =
            CFStringGetCStringPtr(macPath,CFStringGetSystemEncoding());

    cherry_kit::extension_manager *loader =
            cherry_kit::extension_manager::init(
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String(
                                             "/Contents/PlugIns/plexydesk/data/"
                                             )),
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String(
                                             "/Contents/PlugIns/plexydesk/")));
    Q_UNUSED(loader);
    CFRelease(appUrlRef);
    CFRelease(macPath);
    int screen_count = [[NSScreen screens] count];


    for(int i = 0; i < screen_count; i++) {
      DesktopManager *workspace = new DesktopManager();
      m_workspace_list.push_back(workspace);

#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_6
      workspace->set_accelerated_rendering(true);
#endif

      workspace->move_to_screen(i);

      workspace->add_default_controller("classicbackdrop");
      workspace->add_default_controller("plexyclock");
      workspace->add_default_controller("desktopnoteswidget");
      workspace->add_default_controller("folderwidget");
      workspace->add_default_controller("photoframe");
      workspace->add_default_controller("datewidget");
      workspace->add_default_controller("dockwidget");
      workspace->add_default_controller("desksyncwidget");

      workspace->restore_session();

      if (workspace->space_count() <= 0) {
        workspace->add_default_space();
      }

      workspace->expose(0);


#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_5
      HIViewRef _desktopView = reinterpret_cast<HIViewRef>(workspace->winId());
      HIWindowRef _window = HIViewGetWindow(_desktopView);

      if (_window) {
         ChangeWindowAttributes(_window, kWindowNoShadowAttribute, kWindowNoAttributes);  
         SetWindowGroupLevel(GetWindowGroup(_window), kCGDesktopIconWindowLevel);
      } 
#endif

#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_6
      NSView *_desktopView = reinterpret_cast<NSView *>(workspace->winId());
      [[_desktopView window]
              setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces];
      [[_desktopView window] setHasShadow:NO];
      [[_desktopView window] setOpaque:NO];
      [[_desktopView window] setLevel:kCGDesktopIconWindowLevel + 1];
      [[_desktopView window] makeKeyAndOrderFront:_desktopView];
      [[_desktopView window] setBackgroundColor:[NSColor clearColor]];
#endif
      workspace->show();
    }
  }

  ~Runtime() {
    std::for_each(std::begin(m_workspace_list), std::end(m_workspace_list),
                  [&] (DesktopManager *a_manager) {
                  QString::fromStdString(a_manager->workspace_instance_name());
         delete a_manager;
    });

  }

private:
    std::vector<DesktopManager*> m_workspace_list;
};

int main(int argc, char *argv[])
{
  QApplication qtApp(argc, argv);
  Runtime runtime;

#if 0
  NSScreen *screen = [NSScreen mainScreen];
  NSDictionary *description = [screen deviceDescription];
  NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
  CGSize displayPhysicalSize = CGDisplayScreenSize(
              [[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);

  NSLog(@"DPI is %0.2f",
           (displayPixelSize.width / displayPhysicalSize.width) * 25.4f);

  NSLog(@"DPI is %0.2f",
           (displayPixelSize.height / displayPhysicalSize.height) * 25.4f);
#endif

  return qtApp.exec();
}
