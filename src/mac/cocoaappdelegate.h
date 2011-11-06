#import <Cocoa/Cocoa.h>

#include "../mainwindow.h"

@interface CocoaAppDelegate : NSObject<NSApplicationDelegate>
{
    bool _terminationPending;
    MainWindow *_mainWindow;
}

- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication *)sender;
- (void) acceptPendingTermination;
- (void) cancelPendingTermination;

- (void) setMainWindow:(MainWindow *)aMainWindow;

@end
