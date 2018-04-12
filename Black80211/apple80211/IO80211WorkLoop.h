/*
 *  IO80211WorkLoop.h
 *  Family
 *
 *  Created by Pete on 5/31/06.
 *  Copyright 2006 Apple Computer, Inc. All rights reserved.
 *
 */

#include <IOKit/IOWorkLoop.h>

class IO80211WorkLoop : public IOWorkLoop
{
    OSDeclareDefaultStructors( IO80211WorkLoop )
    
public:
    
    static IO80211WorkLoop * workLoop();
    
    virtual IOReturn runAction(Action action, OSObject *target,
                               void *arg0 = 0, void *arg1 = 0,
                               void *arg2 = 0, void *arg3 = 0);
    
    virtual int sleepGate( void * event, UInt32 interuptibleType );
    virtual int sleepGateDeadline( void * event, UInt32 interuptibleType, AbsoluteTime deadline );
    virtual void wakeupGate( void * event, bool oneThread );
};

