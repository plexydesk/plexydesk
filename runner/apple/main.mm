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


bool dockClickHandler(id self,SEL _cmd,...)
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
        bool test = class_addMethod((id)delClass,
                                    sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"),
                                    (IMP)dockClickHandler,"B@:");

        if (!test)
        {
            NSLog(@"Failed to register with your mac");
        }
    }

    NSLog(@"Start PlexyDesk :MacUI \n");

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath,CFStringGetSystemEncoding());

    CFRelease(appUrlRef);
    CFRelease(macPath);

    PlexyDesk::ExtensionManager *loader =
            PlexyDesk::ExtensionManager::instance(
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String("/Contents/share/plexy/ext/groups/")),
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String("/Contents/lib/plexyext/")));
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

    [[_desktopView window] setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces];
    [[_desktopView window] setHasShadow:NO];
    [[_desktopView window] setOpaque:NO];
    [[_desktopView window] setLevel:kCGDesktopIconWindowLevel + 1];
    [[_desktopView window] makeKeyAndOrderFront:_desktopView];
    [[_desktopView window] setBackgroundColor:[NSColor clearColor]];

    workspace.show();

    qtApp.setStyleSheet(QString("QScrollBar:vertical{ border: 2px solid grey; background: #32CC99; height: 15px; margin: 0px 20px 0 20px; }"));

    return qtApp.exec();
}
