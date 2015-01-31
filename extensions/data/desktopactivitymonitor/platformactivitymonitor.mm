#include "platformactivitymonitor.h"

#include <QDebug>
#include <QString>

#include "windowobject.h"

#import <cocoa/cocoa.h>
#import <CoreFoundation/CoreFoundation.h>

class PlatformActivityMonitor::PlatformActivityMonitorPrivate
{
public:
    PlatformActivityMonitorPrivate() {}
    ~PlatformActivityMonitorPrivate() {}

    CFRunLoopTimerRef mTimer;
};

void ActiveSenseTimerCallback(CFRunLoopTimerRef timer, void *info)
{
    //CFRunLoopWakeUp(CFRunLoopGetCurrent());
    CFRunLoopTimerContext TimerContext;
    TimerContext.version = 0;

    CFRunLoopTimerGetContext(timer, &TimerContext);
    ((PlatformActivityMonitor *)TimerContext.info)->timeout();

    NSLog(@"Running the timer");

    /*
    CFStringRef theMode = CFSTR("TimerMode");

    CFRunLoopAddTimer(CFRunLoopGetMain(), timer, theMode);
    CFRunLoopRunInMode(theMode, 2, false);
    */
}

PlatformActivityMonitor::PlatformActivityMonitor(QObject *parent) :
    QObject(parent),
    d(new PlatformActivityMonitorPrivate)
{
    //next();
}

PlatformActivityMonitor::~PlatformActivityMonitor()
{
    delete d;
}

WindowObject PlatformActivityMonitor::updateWindowList()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSMutableArray *windows = (NSMutableArray *)CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly |
                                                                           kCGWindowListExcludeDesktopElements, kCGNullWindowID);
    int i = 0;
    for (NSDictionary *window in windows) {
        QString owner(QString::fromNSString([window objectForKey:@"kCGWindowOwnerName"]));
        QString name(QString::fromNSString([window objectForKey:@"kCGWindowName" ]));
        NSNumber *layer = [window objectForKey:@"kCGWindowLayer"];

        if ([layer intValue] == 0) {
           WindowObject object;
           object.setName(owner);
           object.setTitle(name);
           object.setTime(QTime::currentTime());
           return object;
           break;
        }
    }

    return WindowObject();
}

void PlatformActivityMonitor::next()
{
   //create a run loop;
    CFRunLoopTimerContext TimerContext = {0, this, NULL, NULL, NULL};
    NSLog(@"timer %f", CFAbsoluteTimeGetCurrent());

    d->mTimer = CFRunLoopTimerCreate(kCFAllocatorDefault,
                                  CFAbsoluteTimeGetCurrent() + 1,
                                  1, 0, 0,
                                  ActiveSenseTimerCallback,
                                  &TimerContext);

    CFRunLoopAddTimer(CFRunLoopGetCurrent(), d->mTimer, kCFRunLoopCommonModes);
}

void PlatformActivityMonitor::timeout()
{
    CFRunLoopTimerSetNextFireDate(d->mTimer, CFAbsoluteTimeGetCurrent() + 1);
}
