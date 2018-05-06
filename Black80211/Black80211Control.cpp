/* add your code here */

typedef unsigned int ifnet_ctl_cmd_t;

#include "IONetworkInterface.h"
#include "IONetworkController.h"

#include "Black80211Control.hpp"

OSDefineMetaClassAndStructors(Black80211Control, IO80211Controller);
#define super IO80211Controller

bool Black80211Control::init(OSDictionary *parameters) {
    bool ret = super::init(parameters);
    if (!ret) {
        return ret;
    }
    
    dev = new FakeDevice();
    return true;
}

void Black80211Control::free() {
    delete dev;
    
    super::free();
}

bool Black80211Control::start(IOService *provider) {
    IOLog("ARRRRRRRRRRRRRHHHHHHHHHHHHHHHHHHHH");
    
    if (!super::start(provider))
        return false;
    
    fWorkloop = IO80211WorkLoop::workLoop();
    
    attachInterface((IONetworkInterface**) &fInterface, /* attach to DLIL = */ true);
    
    registerService();
    
    return true;
}

void Black80211Control::stop(IOService *provider) {
    if (fWorkloop) {
        fWorkloop->release();
        fWorkloop = NULL;
    }
    
    super::stop(provider);
}

IOReturn Black80211Control::enable(IONetworkInterface *interface) {
    return kIOReturnSuccess;
}

IOReturn Black80211Control::disable(IONetworkInterface *interface) {
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getHardwareAddress(IOEthernetAddress *addr) {
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getHardwareAddressForInterface(IO80211Interface* netif, IOEthernetAddress* addr) {
    return getHardwareAddress(addr);
}

SInt32 Black80211Control::apple80211Request( UInt32 request_type, int request_number, IO80211Interface* interface, void* data ) {
    if (request_type != SIOCGA80211 && request_type != SIOCSA80211) {
        IOLog("Invalid request");
        return 0;
    }

    IOReturn ret = 0;
    
    IOLog("Black80211: IOCTL t=%u, n=%d", request_type, request_number);
    
    bool isGet = (request_type == SIOCGA80211);
    
    switch (request_number) {
        case APPLE80211_IOC_POWER:
            if( isGet ) {
                ret = getPOWER(interface, (struct apple80211_power_data *)data);
            } else {
                ret = setPOWER(interface, (struct apple80211_power_data *)data);
            }
            break;
        case APPLE80211_IOC_CARD_CAPABILITIES:
            if (isGet) {
                ret = getCARD_CAPABILITIES(interface, (struct apple80211_capability_data *)data);
            }
            break;
    }
    
    return ret;
}

//IOReturn Black80211Control::apple80211Request_SET(int request_number, void* data) {
//    return kIOReturnSuccess;
//}

//IOReturn Black80211Control::apple80211Request_GET(int request_number, void* data) {
//    return kIOReturnSuccess;
//}

IO80211WorkLoop* Black80211Control::getWorkLoop() {
    return fWorkloop;
}

IO80211Interface* Black80211Control::getInterface() {
    return fInterface;
}

UInt32 Black80211Control::outputPacket(mbuf_t m, void *param) {
    freePacket(m);
    return kIOReturnSuccess;
}

IOOutputQueue* Black80211Control::createOutputQueue() {
    
    if (fOutputQueue == 0) {
        fOutputQueue = IOGatedOutputQueue::withTarget(this, getWorkLoop());
    }
    return fOutputQueue;

}


IOReturn Black80211Control::getMaxPacketSize( UInt32 *maxSize ) const { *maxSize = 1500; return kIOReturnSuccess; }
IOReturn Black80211Control::setPromiscuousMode(IOEnetPromiscuousMode mode) { return kIOReturnSuccess; }
IOReturn Black80211Control::setMulticastMode(IOEnetMulticastMode mode) { return kIOReturnSuccess; }
IOReturn Black80211Control::setMulticastList(IOEthernetAddress* addr, UInt32 len) { return kIOReturnSuccess; }
SInt32   Black80211Control::monitorModeSetEnabled(IO80211Interface* interface, bool enabled, UInt32 dlt) { return kIOReturnSuccess; }

const OSString*    Black80211Control::newVendorString    ( ) const    { return OSString::withCString("Voodoo(R)"); }
const OSString*    Black80211Control::newModelString        ( ) const    { return OSString::withCString("Wireless Device(TM)"); }
const OSString*    Black80211Control::newRevisionString    ( ) const    { return OSString::withCString("1.0"); }



//
// IOCTL
//

IOReturn Black80211Control::getPOWER(IO80211Interface *interface, struct apple80211_power_data *pd) {
    pd->version = APPLE80211_VERSION;
    pd->num_radios = 3;
    pd->power_state[0] = dev->powerState();
    pd->power_state[1] = dev->powerState();
    pd->power_state[2] = dev->powerState();

    return kIOReturnSuccess;
}

IOReturn Black80211Control::setPOWER(IO80211Interface *interface, struct apple80211_power_data *pd) {
    if (pd->num_radios > 0) {
        dev->setPowerState(pd->power_state[0]);
    }
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getCARD_CAPABILITIES(IO80211Interface *interface, struct apple80211_capability_data *cd) {
    cd->version = APPLE80211_VERSION;
    cd->capabilities[0] = 0xab;
    cd->capabilities[1] = 0x7e;
    return kIOReturnSuccess;
}


