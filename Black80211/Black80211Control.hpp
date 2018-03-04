/* add your code here */
#ifndef net80211_Voodoo80211Device_h
#define net80211_Voodoo80211Device_h

#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/network/IOGatedOutputQueue.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOLocks.h>

#include <IOKit/network/IOGatedOutputQueue.h>

#include "apple80211/IO80211Controller.h"
#include "apple80211/IO80211WorkLoop.h"
#include "apple80211/IO80211Interface.h"

class Black80211Control : public IO80211Controller {
    
    OSDeclareDefaultStructors(Black80211Control)
    
public:
    bool init(OSDictionary* parameters);
    void free();
    bool start(IOService *provider);
    void stop(IOService *provider);
    virtual IO80211WorkLoop* getWorkLoop();
    IOReturn        registerWithPolicyMaker ( IOService* policyMaker );
    SInt32          apple80211RequestIoctl  ( UInt32 request_type, int request_number, IO80211Interface* interface, void* data );
    IOReturn        apple80211Request_SET   ( int request_number, void* data );
    IOReturn        apple80211Request_GET   ( int request_number, void* data );
    IOReturn        enable           ( IONetworkInterface* aNetif );
    IOReturn        disable          ( IONetworkInterface* aNetif );
    IOOutputQueue*        createOutputQueue    ( );
    virtual UInt32        outputPacket        ( mbuf_t m, void* param );
    IOReturn        getMaxPacketSize    ( UInt32 *maxSize ) const;
    const OSString*        newVendorString        ( ) const;
    const OSString*        newModelString        ( ) const;
    const OSString*        newRevisionString    ( ) const;
    IOReturn        getHardwareAddressForInterface( IO80211Interface* netif, IOEthernetAddress* addr );
    IOReturn        getHardwareAddress    ( IOEthernetAddress* addr );
    virtual IOReturn    setPromiscuousMode    ( IOEnetPromiscuousMode mode );
    virtual IOReturn    setMulticastMode    ( IOEnetMulticastMode mode );
    virtual IOReturn    setMulticastList    ( IOEthernetAddress* addr, UInt32 len );
    virtual SInt32        monitorModeSetEnabled    ( IO80211Interface * interface, bool enabled, UInt32 dlt );
    
    
private:
    IO80211WorkLoop *fWorkloop;
    IO80211Interface* fInterface;
    IOGatedOutputQueue* fOutputQueue;
    IOCommandGate*        fCommandGate;
    IOTimerEventSource*     fTimer;
    
protected:
    IO80211Interface* getInterface();
//private:
//    OSMetaClassDeclareReservedUnused( Black80211Control,  0);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  1);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  2);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  3);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  4);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  5);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  6);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  7);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  8);
//    OSMetaClassDeclareReservedUnused( Black80211Control,  9);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 10);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 11);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 12);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 13);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 14);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 15);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 16);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 17);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 18);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 19);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 20);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 21);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 22);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 23);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 24);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 25);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 26);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 27);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 28);
//    OSMetaClassDeclareReservedUnused( Black80211Control, 29);
    

};

#endif
