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
    IOLog("Black80211: Start");
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
        IOLog("Black80211: Invalid request");
        return 0;
    }

    IOReturn ret = 0;
    
    bool isGet = (request_type == SIOCGA80211);
    
#define IOCTL(REQ_TYPE, REQ, DATA_TYPE) \
if (REQ_TYPE == SIOCGA80211) { \
ret = get##REQ(interface, (struct DATA_TYPE *)data); \
} else { \
ret = set##REQ(interface, (struct DATA_TYPE *)data); \
}
    
#define IOCTL_GET(REQ_TYPE, REQ, DATA_TYPE) \
if (REQ_TYPE == SIOCGA80211) { \
    ret = get##REQ(interface, (struct DATA_TYPE *)data); \
}

    
    
    
    IOLog("Black80211: IOCTL %s(%d)", isGet ? "get" : "set", request_number);
    
    switch (request_number) {
        case APPLE80211_IOC_HARDWARE_VERSION:
            IOCTL_GET(request_type, HARDWARE_VERSION, apple80211_version_data);
            break;
        case APPLE80211_IOC_DRIVER_VERSION:
            IOCTL_GET(request_type, DRIVER_VERSION, apple80211_version_data);
            break;
        case APPLE80211_IOC_LOCALE:
            IOCTL_GET(request_type, LOCALE, apple80211_locale_data);
            break;
        case APPLE80211_IOC_COUNTRY_CODE:
            IOCTL_GET(request_type, COUNTRY_CODE, apple80211_country_code_data);
            break;
        case APPLE80211_IOC_PHY_MODE:
            IOCTL_GET(request_type, PHY_MODE, apple80211_phymode_data);
            break;
        case APPLE80211_IOC_SUPPORTED_CHANNELS:
            IOCTL_GET(request_type, SUPPORTED_CHANNELS, apple80211_sup_channel_data);
            break;
        case APPLE80211_IOC_OP_MODE:
            IOCTL_GET(request_type, OP_MODE, apple80211_opmode_data);
            break;
        case APPLE80211_IOC_SSID:
            IOCTL(request_type, SSID, apple80211_ssid_data);
            break;
        case APPLE80211_IOC_STATE:
            IOCTL_GET(request_type, STATE, apple80211_state_data);
            break;
        case APPLE80211_IOC_POWER:
            IOCTL(request_type, POWER, apple80211_power_data);
            break;
        case APPLE80211_IOC_CARD_CAPABILITIES:
            IOCTL_GET(request_type, CARD_CAPABILITIES, apple80211_capability_data);
            break;
    }
#undef IOCTL
    
    return ret;
}

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
    if (!pd) {
        return kIOReturnSuccess;
    }
    pd->version = APPLE80211_VERSION;
    pd->num_radios = 3;
    pd->power_state[0] = dev->powerState();
    pd->power_state[1] = dev->powerState();
    pd->power_state[2] = dev->powerState();

    return kIOReturnSuccess;
}

IOReturn Black80211Control::setPOWER(IO80211Interface *interface, struct apple80211_power_data *pd) {
    if (!pd) {
        return kIOReturnSuccess;
    }
    if (pd->num_radios > 0) {
        dev->setPowerState(pd->power_state[0]);
    }
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getCARD_CAPABILITIES(IO80211Interface *interface, struct apple80211_capability_data *cd) {
    if (!cd) {
        return kIOReturnSuccess;
    }
    cd->version = APPLE80211_VERSION;
    cd->capabilities[0] = 0xab;
    cd->capabilities[1] = 0x7e;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getSSID(IO80211Interface *interface, struct apple80211_ssid_data *sd) {
    if (!sd) {
        return kIOReturnSuccess;
    }
    memset(sd, 0, sizeof(*sd));
    sd->version = APPLE80211_VERSION;
    strncpy((char*)sd->ssid_bytes, "anetwork", sizeof(sd->ssid_bytes));
    sd->ssid_len = (UInt32)strlen("anetwork");
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setSSID(IO80211Interface *interface, struct apple80211_ssid_data *sd) {
    return kIOReturnSuccess;
}


IOReturn Black80211Control::getSTATE(IO80211Interface *interface, struct apple80211_state_data *sd) {
    if (!sd) {
        return kIOReturnSuccess;
    }
    sd->version = APPLE80211_VERSION;
    sd->state = APPLE80211_S_INIT;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getOP_MODE(IO80211Interface *interface, struct apple80211_opmode_data *od) {
    if (!od) {
        return kIOReturnSuccess;
    }
    od->version = APPLE80211_VERSION;
    od->op_mode = APPLE80211_M_STA;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getSUPPORTED_CHANNELS(IO80211Interface *interface, struct apple80211_sup_channel_data *ad) {
    if (!ad) {
        return kIOReturnSuccess;
    }
    ad->version = APPLE80211_VERSION;
    ad->num_channels = 13;
    
    int i;
    for(i=1; i<=ad->num_channels; i++) {
        ad->supported_channels[i-1].version = APPLE80211_VERSION;
        ad->supported_channels[i-1].channel = i;
        ad->supported_channels[i-1].flags   = APPLE80211_C_FLAG_2GHZ;
    }
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getHARDWARE_VERSION(IO80211Interface *interface, struct apple80211_version_data *hv) {
    if (!hv) {
        return kIOReturnSuccess;
    }
    hv->version = APPLE80211_VERSION;
    strncpy(hv->string, "Ferrum 0", sizeof(hv->string));
    hv->string_len = strlen("Ferrum 0");
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getDRIVER_VERSION(IO80211Interface *interface, struct apple80211_version_data *hv) {
    if (!hv) {
        return kIOReturnSuccess;
    }
    hv->version = APPLE80211_VERSION;
    strncpy(hv->string, "Version 0.0", sizeof(hv->string));
    hv->string_len = strlen("Version 0.0");
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getLOCALE(IO80211Interface *interface, struct apple80211_locale_data *ld) {
    if (!ld) {
        return kIOReturnSuccess;
    }
    ld->version = APPLE80211_VERSION;
    ld->locale  = APPLE80211_LOCALE_FCC;
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getCOUNTRY_CODE(IO80211Interface *interface, struct apple80211_country_code_data *cd) {
    if (!cd) {
        return kIOReturnSuccess;
    }
    cd->version = APPLE80211_VERSION;
    strncpy((char*)cd->cc, "us", sizeof(cd->cc));
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getPHY_MODE(IO80211Interface *interface, struct apple80211_phymode_data *pd) {
    if (!pd) {
        return kIOReturnSuccess;
    }
    pd->version = APPLE80211_VERSION;
    pd->phy_mode = APPLE80211_MODE_11A | APPLE80211_MODE_11B | APPLE80211_MODE_11G;
    pd->active_phy_mode = APPLE80211_MODE_11B;
    return kIOReturnSuccess;
}

