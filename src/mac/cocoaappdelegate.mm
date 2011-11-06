#include <QtGui>
#include <QtDebug>

#import "cocoaappdelegate.h"

@implementation CocoaAppDelegate

- (id) init
{
    if (!(self = [super init]))
        return nil;

    _terminationPending = false;
    _mainWindow = NULL;

    return self;
}

- (void) setMainWindow:(MainWindow *)aMainWindow
{
    _mainWindow = aMainWindow;
}

- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication *)sender
{
    Q_UNUSED(sender);

    qDebug() << "applicationShouldTerminate";

    _terminationPending = true;
    _mainWindow->cocoaCommitDataHandler();
    qDebug() << "applicationShouldTerminate returned from cocoaCommitDataHandler().";

    return NSTerminateLater;
}

- (void) acceptPendingTermination
{
    qDebug() << "acceptPendingTermination. pending =" << _terminationPending;
    if (!_terminationPending)
        return;
    _terminationPending = false;
    [NSApp replyToApplicationShouldTerminate:YES];
}

- (void) cancelPendingTermination
{
    qDebug() << "cancelPendingTermination. pending =" << _terminationPending;
    if (!_terminationPending)
        return;
    _terminationPending = false;
    [NSApp replyToApplicationShouldTerminate:NO];
}


@end
