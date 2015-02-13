/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <QtGui/QtGui>
#include <QtWidgets/QMacNativeWidget>

#include <QApplication>
#include <QtCore>
#include <QIcon>
#include <extensionmanager.h>
#include <plexy.h>
#include <workspace.h>
#include <space.h>
#include <plexyconfig.h>
#include "desktopmanager.h"

#include <objc/objc.h>
#include <objc/message.h>


bool OnDockMousePressCallback(id self,SEL _cmd,...)
{
    Q_UNUSED(self)
    Q_UNUSED(_cmd)

    [[NSWorkspace sharedWorkspace] hideOtherApplications];
    return YES;
}

int main(int argc, char *argv[])
{
    QApplication qtApp(argc, argv);

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
                    (IMP)OnDockMousePressCallback,"B@:");

        if (!classMethodFunc) {
            NSLog(@"Failed to register with your mac");
        }
    }

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath =
            CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr =
            CFStringGetCStringPtr(macPath,CFStringGetSystemEncoding());

    CFRelease(appUrlRef);
    CFRelease(macPath);

    UIKit::ExtensionManager *loader =
            UIKit::ExtensionManager::init(
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String(
                                             "/Contents/PlugIns/plexydesk/data/"
                                             )),
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String(
                                             "/Contents/PlugIns/plexydesk/")));
    Q_UNUSED(loader);


    DesktopManager workspace;
    workspace.addDefaultController("classicbackdrop");
    workspace.addDefaultController("dockwidget");
    workspace.addDefaultController("plexyclock");
    workspace.addDefaultController("desktopnoteswidget");
    workspace.addDefaultController("folderwidget");
    workspace.addDefaultController("photoframe");

    workspace.restoreSession();

    if (workspace.spaceCount() <= 0) {
        workspace.addSpace();
    }

    NSView *_desktopView = reinterpret_cast<NSView *>(workspace.winId());

    [[_desktopView window]
            setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces];
    [[_desktopView window] setHasShadow:NO];
    [[_desktopView window] setOpaque:NO];
    [[_desktopView window] setLevel:kCGDesktopIconWindowLevel + 1];
    [[_desktopView window] makeKeyAndOrderFront:_desktopView];
    [[_desktopView window] setBackgroundColor:[NSColor clearColor]];

    workspace.show();

    qtApp.setStyleSheet(
                QString("QScrollBar:vertical{ border: 2px solid grey;"
                        " background: #32CC99; height: 15px; margin: 0px 20px"
                        " 0 20px; }"));

    return qtApp.exec();
}
