#include "appleidletimedetector.h"

#include <QDebug>

#include <QTime>
#include <QElapsedTimer>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/IOKitLib.h>
#include <Cocoa/Cocoa.h>

#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

static CFTimeInterval startTime = 0.0;// = CACurrentMediaTime();

//c callback to recive sleep/wakeup notifications.
static void appleSleepCallback(void *refCon, io_service_t service, natural_t messageType, void *messageArgument )
{
    if (refCon) {
        AppleIdleTimeDetector *detector = static_cast<AppleIdleTimeDetector*>(refCon);
        if (detector) {
           switch(messageType) {
           case kIOMessageSystemWillSleep:
               /* perform sleep actions */
               startTime = CACurrentMediaTime();
               detector->setStartTime(QDateTime::currentDateTime());
               break;

           case kIOMessageSystemHasPoweredOn:
               /* perform wakeup actions */
               qDebug("%f", CACurrentMediaTime() - startTime);
               qDebug() << Q_FUNC_INFO << QDateTime::currentDateTime().secsTo(detector->startTime());
               detector->setHardwareSleepDuration(CACurrentMediaTime() - startTime);
               break;

           case kIOMessageSystemWillRestart:
               /* perform restart actions */
               break;

           case kIOMessageSystemWillPowerOff:
               /* perform shutdown actions */
               break;
           }
        }
    }
}

class AppleIdleTimeDetector::AppleIdleTimeDetectorPrivate
{
public:
    AppleIdleTimeDetectorPrivate() {}
    ~AppleIdleTimeDetectorPrivate() {}

    mach_port_t   mIOPort;
    io_iterator_t mIOIterator;
    io_object_t   mIOObject;
    io_connect_t  mRootPort;
    io_object_t mNotifierObject;
    IONotificationPortRef mNotifyPortRef;
    quint64 mSleepTime;
    QDateTime mStartTime;

    bool ioStatus;
};

AppleIdleTimeDetector::AppleIdleTimeDetector(QObject *parent) : QObject(parent),
    d(new AppleIdleTimeDetectorPrivate)
{
    d->ioStatus = false;
    d->mSleepTime = 0.0f;
}

AppleIdleTimeDetector::~AppleIdleTimeDetector()
{
    delete d;
}

void AppleIdleTimeDetector::init()
{
    NSLog(@"Start Idle Time Detector Process");
    kern_return_t status;

    status = IOMasterPort(MACH_PORT_NULL, &d->mIOPort);

    if (status != KERN_SUCCESS) {
        qWarning() << Q_FUNC_INFO << "Failed to get IO Status from Mach Kernel";
        d->ioStatus = false;
        return;
    }

    status = IOServiceGetMatchingServices(d->mIOPort, IOServiceMatching( "IOHIDSystem" ),&d->mIOIterator);

    if (status != KERN_SUCCESS) {
        qWarning() << Q_FUNC_INFO << "Failed to connect to service : IOHIDSystem";
        d->ioStatus = false;
        return;
    }

    d->mIOObject = IOIteratorNext(d->mIOIterator);

    if (d->mIOObject == 0) {
        qWarning() << Q_FUNC_INFO << "Failed to iterate IO Devices";
        d->ioStatus = false;
        return;
    }

    NSLog(@"User space connection to Kernel Established");

    void *refCon = this;

    d->mRootPort = IORegisterForSystemPower(refCon, &d->mNotifyPortRef, appleSleepCallback, &d->mNotifierObject);

    if (d->mRootPort == 0 ) {
        qDebug() << Q_FUNC_INFO << "Error registering for Sleep and Wakeup cycles.";
    } else {
        qDebug() << Q_FUNC_INFO << "KIOT ready to recive notifications from the system";
        CFRunLoopAddSource (CFRunLoopGetCurrent(),
                                IONotificationPortGetRunLoopSource(d->mNotifyPortRef),
                                kCFRunLoopDefaultMode);
    }
}

quint64 AppleIdleTimeDetector::idleTime() const
{
   kern_return_t status;
   CFTypeRef idle;
   CFTypeID type;
   uint64_t time;
   CFMutableDictionaryRef properties;

   properties = NULL;

   status = IORegistryEntryCreateCFProperties(d->mIOObject, &properties, kCFAllocatorDefault, 0);

   if (status != KERN_SUCCESS)
       return 0;

   idle = CFDictionaryGetValue(properties, CFSTR("HIDIdleTime"));

   if (!idle) {
       CFRelease(properties);
       qWarning() << Q_FUNC_INFO << "HIDDleTime has no properies map";

       return 0;
   }

   type = CFGetTypeID(idle);

   if(type == CFDataGetTypeID()) {
       CFDataGetBytes( ( CFDataRef )idle, CFRangeMake( 0, sizeof( time ) ), ( UInt8 * )&time );
   } else if (type == CFNumberGetTypeID()) {
       CFNumberGetValue( ( CFNumberRef )idle, kCFNumberSInt64Type, &time );
   } else {
       CFRelease( idle );
       CFRelease( ( CFTypeRef )properties );
       qWarning() << Q_FUNC_INFO << "Error getting property time from the IO registry";
       return 0;
   }

   CFRelease(properties);
   CFRelease(idle);

   return time >> 30;
}

void AppleIdleTimeDetector::setHardwareSleepDuration(float time)
{
    d->mSleepTime = time;
    Q_EMIT hardwareAwake();
}

float AppleIdleTimeDetector::hardwareSleepDuraiton()
{
    return d->mSleepTime;
}

void AppleIdleTimeDetector::setStartTime(const QDateTime &time)
{
    d->mStartTime = time;
}

QDateTime AppleIdleTimeDetector::startTime()
{
    return d->mStartTime;
}
