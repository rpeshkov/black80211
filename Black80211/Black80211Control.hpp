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

#include "apple80211.h"

class Black80211Control : public IO80211Controller {
    
    OSDeclareDefaultStructors(Black80211Control)
    
public:
    bool init(OSDictionary* parameters);
    void free();
    bool start(IOService *provider);
    void stop(IOService *provider);
    virtual IO80211WorkLoop* getWorkLoop();
    //SInt32          apple80211RequestIoctl  ( UInt32 request_type, int request_number, IO80211Interface* interface, void* data );
    //IOReturn        apple80211Request_SET   ( int request_number, void* data );
    //IOReturn        apple80211Request_GET   ( int request_number, void* data );
    SInt32          apple80211Request  ( UInt32 request_type, int request_number, IO80211Interface* interface, void* data );
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
};

#endif
