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

#include "FakeDevice.hpp"

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
    IOReturn getPOWER(IO80211Interface *interface, struct apple80211_power_data *pd);
    IOReturn setPOWER(IO80211Interface *interface, struct apple80211_power_data *pd);
    IOReturn getCARD_CAPABILITIES(IO80211Interface *interface, struct apple80211_capability_data *cd);
    IOReturn getSSID(IO80211Interface *interface, struct apple80211_ssid_data *sd);
    IOReturn setSSID(IO80211Interface *interface, struct apple80211_ssid_data *sd);
    IOReturn getSTATE(IO80211Interface *interface, struct apple80211_state_data *sd);
    IOReturn getOP_MODE(IO80211Interface *interface, struct apple80211_opmode_data *od);
    IOReturn getSUPPORTED_CHANNELS(IO80211Interface *interface, struct apple80211_sup_channel_data *ad);
    IOReturn getHARDWARE_VERSION(IO80211Interface *interface, struct apple80211_version_data *hv);
    IOReturn getDRIVER_VERSION(IO80211Interface *interface, struct apple80211_version_data *hv);
    IOReturn getLOCALE(IO80211Interface *interface, struct apple80211_locale_data *ld);
    IOReturn getCOUNTRY_CODE(IO80211Interface *interface, struct apple80211_country_code_data *cd);
    IOReturn getPHY_MODE(IO80211Interface *interface, struct apple80211_phymode_data *pd);

    
    IO80211WorkLoop *fWorkloop;
    IO80211Interface* fInterface;
    IOGatedOutputQueue* fOutputQueue;
    IOCommandGate*        fCommandGate;
    IOTimerEventSource*     fTimer;
    
    FakeDevice *dev;
    
protected:
    IO80211Interface* getInterface();
};

#endif
