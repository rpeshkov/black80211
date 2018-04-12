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

struct ieee80211com {
    
};

struct ieee80211_node {
    
};

struct ieee80211_key {
    
};

struct ieee80211_rxinfo {
    
};

enum ieee80211_state {
    IEEE80211_S_INIT    = 0,    /* default state */
    IEEE80211_S_SCAN    = 1,    /* scanning */
    IEEE80211_S_AUTH    = 2,    /* try to authenticate */
    IEEE80211_S_ASSOC    = 3,    /* try to assoc */
    IEEE80211_S_RUN        = 4    /* associated */
};

class Black80211Control : public IO80211Controller {
    
    OSDeclareDefaultStructors(Black80211Control)
    
public:
    static IOReturn        tsleepHandler(OSObject* owner, void* arg0, void* arg1, void* arg2, void* arg3);
    
    bool init(OSDictionary* parameters);
    void free();
    bool start(IOService *provider);
    void stop(IOService *provider);
    virtual IO80211WorkLoop* getWorkLoop();
    IOReturn        registerWithPolicyMaker    ( IOService* policyMaker );
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
    
    virtual struct ieee80211com* getIeee80211com() { return 0; }
    
    virtual bool    device_attach(void *) { return false; }
    virtual int    device_detach(int) { return 1; }
    virtual int    device_activate(int) { return 1; }
    virtual void    device_netreset() { return; }
    virtual bool    device_powered_on() { return false; }
    virtual void    ieee80211_newassoc(struct ieee80211com *, struct ieee80211_node *, int) {}
    virtual void    ieee80211_updateslot(struct ieee80211com *) {}
    virtual void    ieee80211_updateedca(struct ieee80211com *) {}
    virtual void    ieee80211_set_tim(struct ieee80211com *, int, int) {}
    virtual int    ieee80211_ampdu_tx_start(struct ieee80211com *, struct ieee80211_node *, u_int8_t) { return 0; }
    virtual void    ieee80211_ampdu_tx_stop(struct ieee80211com *, struct ieee80211_node *, u_int8_t) {}
    virtual int    ieee80211_ampdu_rx_start(struct ieee80211com *, struct ieee80211_node *, u_int8_t) { return 0; }
    virtual void    ieee80211_ampdu_rx_stop(struct ieee80211com *, struct ieee80211_node *, u_int8_t) {}
    virtual struct ieee80211_node *ieee80211_node_alloc(struct ieee80211com *) {return 0;}
    virtual void ieee80211_node_free(struct ieee80211com *, struct ieee80211_node *) {}
    virtual void ieee80211_node_copy(struct ieee80211com *, struct ieee80211_node *, const struct ieee80211_node *) {}
    virtual u_int8_t ieee80211_node_getrssi(struct ieee80211com *, const struct ieee80211_node *) {return 0;}
    virtual void ieee80211_node_join(struct ieee80211com *, struct ieee80211_node *, int) {}
    virtual void ieee80211_node_leave(struct ieee80211com *, struct ieee80211_node *) {}
    virtual int ieee80211_set_key(struct ieee80211com *, struct ieee80211_node *, struct ieee80211_key *) {return 0;}
    virtual void ieee80211_delete_key(struct ieee80211com *, struct ieee80211_node *, struct ieee80211_key *) {}
    virtual void ieee80211_recv_mgmt(struct ieee80211com *, mbuf_t, struct ieee80211_node *, struct ieee80211_rxinfo *, int) {}
    virtual int ieee80211_send_mgmt(struct ieee80211com *, struct ieee80211_node *, int, int, int) {return 0;}
    virtual int ieee80211_newstate(struct ieee80211com *, enum ieee80211_state, int) {return 0;}
};

#endif
