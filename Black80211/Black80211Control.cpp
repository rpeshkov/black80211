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
    
    fInterface->registerService();
    
    mediumDict = OSDictionary::withCapacity(MEDIUM_TYPE_INVALID + 1);
    addMediumType(kIOMediumIEEE80211None,  0,  MEDIUM_TYPE_NONE);
    addMediumType(kIOMediumIEEE80211Auto,  0,  MEDIUM_TYPE_AUTO);
    addMediumType(kIOMediumIEEE80211DS1,   1000000, MEDIUM_TYPE_1MBIT);
    addMediumType(kIOMediumIEEE80211DS2,   2000000, MEDIUM_TYPE_2MBIT);
    addMediumType(kIOMediumIEEE80211DS5,   5500000, MEDIUM_TYPE_5MBIT);
    addMediumType(kIOMediumIEEE80211DS11, 11000000, MEDIUM_TYPE_11MBIT);
    addMediumType(kIOMediumIEEE80211,     54000000, MEDIUM_TYPE_54MBIT, "OFDM54");
    //addMediumType(kIOMediumIEEE80211OptionAdhoc, 0, MEDIUM_TYPE_ADHOC,"ADHOC");
    
    publishMediumDictionary(mediumDict);
    setCurrentMedium(mediumTable[MEDIUM_TYPE_AUTO]);
    setSelectedMedium(mediumTable[MEDIUM_TYPE_AUTO]);
    setLinkStatus(kIONetworkLinkValid, mediumTable[MEDIUM_TYPE_AUTO]);

    
    registerService();
    return true;
}

bool Black80211Control::addMediumType(UInt32 type, UInt32 speed, UInt32 code, char* name) {
    IONetworkMedium * medium;
    bool              ret = false;
    
    medium = IONetworkMedium::medium(type, speed, 0, code, name);
    if (medium) {
        ret = IONetworkMedium::addMedium(mediumDict, medium);
        if (ret)
            mediumTable[code] = medium;
        medium->release();
    }
    return ret;
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
    addr->bytes[0] = 0xAA;
    addr->bytes[1] = 0x99;
    addr->bytes[2] = 0x88;
    addr->bytes[3] = 0x77;
    addr->bytes[4] = 0x66;
    addr->bytes[5] = 0x55;
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
#define IOCTL_SET(REQ_TYPE, REQ, DATA_TYPE) \
if (REQ_TYPE == SIOCSA80211) { \
    ret = set##REQ(interface, (struct DATA_TYPE *)data); \
}
    
    
    IOLog("Black80211: IOCTL %s(%d)", isGet ? "get" : "set", request_number);
    
    
    
    switch (request_number) {
        case APPLE80211_IOC_SSID: // 1
            IOCTL(request_type, SSID, apple80211_ssid_data);
            break;
        case APPLE80211_IOC_AUTH_TYPE: // 2
            IOCTL_GET(request_type, AUTH_TYPE, apple80211_authtype_data);
            break;
        case APPLE80211_IOC_CHANNEL: // 4
            IOCTL_GET(request_type, CHANNEL, apple80211_channel_data);
            break;
        case APPLE80211_IOC_TXPOWER: // 7
            IOCTL_GET(request_type, TXPOWER, apple80211_txpower_data);
            break;
        case APPLE80211_IOC_RATE: // 8
            IOCTL_GET(request_type, RATE, apple80211_rate_data);
            break;
        case APPLE80211_IOC_BSSID: // 9
            IOCTL_GET(request_type, BSSID, apple80211_bssid_data);
            break;
        case APPLE80211_IOC_SCAN_REQ: // 10
            IOCTL_SET(request_type, SCAN_REQ, apple80211_scan_data);
            break;
        case APPLE80211_IOC_SCAN_RESULT: // 11
            IOCTL_GET(request_type, SCAN_RESULT, apple80211_scan_result*);
            break;
        case APPLE80211_IOC_CARD_CAPABILITIES: // 12
            IOCTL_GET(request_type, CARD_CAPABILITIES, apple80211_capability_data);
            break;
        case APPLE80211_IOC_STATE: // 13
            IOCTL_GET(request_type, STATE, apple80211_state_data);
            break;
        case APPLE80211_IOC_PHY_MODE: // 14
            IOCTL_GET(request_type, PHY_MODE, apple80211_phymode_data);
            break;
        case APPLE80211_IOC_OP_MODE: // 15
            IOCTL_GET(request_type, OP_MODE, apple80211_opmode_data);
            break;
        case APPLE80211_IOC_RSSI: // 16
            IOCTL_GET(request_type, RSSI, apple80211_rssi_data);
            break;
        case APPLE80211_IOC_NOISE: // 17
            IOCTL_GET(request_type, NOISE, apple80211_noise_data);
            break;
        case APPLE80211_IOC_POWER: // 19
            IOCTL(request_type, POWER, apple80211_power_data);
            break;
        case APPLE80211_IOC_ASSOCIATE: // 20
            IOCTL_SET(request_type, ASSOCIATE, apple80211_assoc_data);
            break;
        case APPLE80211_IOC_SUPPORTED_CHANNELS: // 27
            IOCTL_GET(request_type, SUPPORTED_CHANNELS, apple80211_sup_channel_data);
            break;
        case APPLE80211_IOC_LOCALE: // 28
            IOCTL_GET(request_type, LOCALE, apple80211_locale_data);
            break;
        case APPLE80211_IOC_TX_ANTENNA: // 37
            IOCTL_GET(request_type, TX_ANTENNA, apple80211_antenna_data);
            break;
        case APPLE80211_IOC_ANTENNA_DIVERSITY: // 39
            IOCTL_GET(request_type, ANTENNA_DIVERSITY, apple80211_antenna_data);
            break;
        case APPLE80211_IOC_DRIVER_VERSION: // 43
            IOCTL_GET(request_type, DRIVER_VERSION, apple80211_version_data);
            break;
        case APPLE80211_IOC_HARDWARE_VERSION: // 44
            IOCTL_GET(request_type, HARDWARE_VERSION, apple80211_version_data);
            break;
        case APPLE80211_IOC_COUNTRY_CODE: // 51
            IOCTL_GET(request_type, COUNTRY_CODE, apple80211_country_code_data);
            break;
        case APPLE80211_IOC_WOW_PARAMETERS: // 69
            break;
        case APPLE80211_IOC_TX_CHAIN_POWER: // 108
            break;
        case APPLE80211_IOC_THERMAL_THROTTLING: // 111
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

IOReturn Black80211Control::getAUTH_TYPE(IO80211Interface *interface, struct apple80211_authtype_data *ad)
{
    
    ad->version = APPLE80211_VERSION;
    ad->authtype_lower = APPLE80211_AUTHTYPE_OPEN;    //    open at this moment
    ad->authtype_upper = APPLE80211_AUTHTYPE_NONE;    //    NO upper AUTHTYPE
    return 0;
}


IOReturn Black80211Control::getPOWER(IO80211Interface *interface, struct apple80211_power_data *pd) {
    if (!pd) {
        return kIOReturnSuccess;
    }
    pd->version = APPLE80211_VERSION;
    pd->num_radios = 1;
    pd->power_state[0] = dev->powerState();
//    pd->power_state[1] = dev->powerState();
//    pd->power_state[2] = dev->powerState();

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
//    strncpy((char*)sd->ssid_bytes, "anetwork", sizeof(sd->ssid_bytes));
//    sd->ssid_len = (UInt32)strlen("anetwork");
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
    sd->state = dev->state();
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setSTATE(IO80211Interface *interface, struct apple80211_state_data *sd) {
    dev->setState(sd->state);
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
    ad->num_channels = 1;
    
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
    strncpy(hv->string, "Broadcom BCM43xx 1.0 (7.21.171.133.1a2)", sizeof(hv->string));
    hv->string_len = strlen("Broadcom BCM43xx 1.0 (7.21.171.133.1a2)");
    
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
    strncpy((char*)cd->cc, "CZ", sizeof(cd->cc));
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

IOReturn Black80211Control::setSCAN_REQ(IO80211Interface *interface, struct apple80211_scan_data *sd) {
    dev->setState(APPLE80211_S_SCAN);
    IOLog("Black80211. Scan requested. Type: %d\n", sd->scan_type);

    if (interface) {
        interface->postMessage(APPLE80211_M_SCAN_DONE);
    }
    
    dev->setState(APPLE80211_S_INIT);
    
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getSCAN_RESULT(IO80211Interface *interface, struct apple80211_scan_result **sr) {
    
    if (*sr) {
        return APPLE80211_M_RSN_AUTH_SUCCESS;
    }
    
    struct apple80211_scan_result *result = (struct apple80211_scan_result *)IOMalloc(sizeof(struct apple80211_scan_result));

    result->version = APPLE80211_VERSION;
    result->asr_age = 1;
    result->asr_beacon_int = 0;
    result->asr_bssid[0] = 0xCE;
    result->asr_bssid[1] = 0xDC;
    result->asr_bssid[2] = 0xBA;
    result->asr_bssid[3] = 0x98;
    result->asr_bssid[4] = 0x76;
    result->asr_bssid[5] = 0x54;
    result->asr_nrates = 1;
    result->asr_rates[0] = 54;
    result->asr_rssi = 60;
    result->asr_noise = 60;
    result->asr_cap = 0xAB;

    result->asr_channel.channel = 1;
    result->asr_channel.version = APPLE80211_VERSION;
    result->asr_channel.flags = APPLE80211_C_FLAG_2GHZ;

     strncpy((char*)result->asr_ssid, "some_network", sizeof(result->asr_ssid));
     result->asr_ssid_len = strlen("some_network");

     result->asr_ie_len = 0;
    result->asr_ie_data = NULL;
    
    sr[0] = result;
    sr[1] = NULL;
    
    return 0;
}

IOReturn Black80211Control::getBSSID(IO80211Interface *interface, struct apple80211_bssid_data *bd) {

    memset(bd, 0, sizeof(*bd));
    
    bd->version = APPLE80211_VERSION;
//    bd->bssid.octet[0] = 0xFE;
//    bd->bssid.octet[1] = 0xDC;
//    bd->bssid.octet[2] = 0xBA;
//    bd->bssid.octet[3] = 0x98;
//    bd->bssid.octet[4] = 0x76;
//    bd->bssid.octet[5] = 0x54;
//
    return 0;
}

IOReturn Black80211Control::getTX_ANTENNA(IO80211Interface *interface, apple80211_antenna_data *ad)
{
    ad->version = APPLE80211_VERSION;
    ad->num_radios = 1;
    ad->antenna_index[0] = 1;
//    ad->antenna_index[1] = 1;
//    ad->antenna_index[2] = 1;
    return kIOReturnSuccess;
}


IOReturn Black80211Control::getANTENNA_DIVERSITY(IO80211Interface *interface, apple80211_antenna_data *ad)
{
    ad->version = APPLE80211_VERSION;
    ad->num_radios = 1;
    ad->antenna_index[0] = 1;
//    ad->antenna_index[1] = 1;
//    ad->antenna_index[2] = 1;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getCHANNEL(IO80211Interface *interface, struct apple80211_channel_data *cd)
{
    //    IOLog("getCHANNEL c:%d f:%d\n",cd->channel.channel,cd->channel.flags);
    cd->version = APPLE80211_VERSION;
    cd->channel.version = APPLE80211_VERSION;
    cd->channel.channel = 1;
    cd->channel.flags = APPLE80211_C_FLAG_2GHZ;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setASSOCIATE(IO80211Interface *interface,struct apple80211_assoc_data *ad)
{
    IOLog("setASSOCIATE \n");
    
//    if (interface)
//        interface->postMessage(APPLE80211_IOC_SCAN_RESULT);
    return 0;
}

IOReturn Black80211Control::getNOISE(IO80211Interface *interface,struct apple80211_noise_data *nd) {
    nd->version = APPLE80211_VERSION;
    nd->num_radios = 1;
    nd->noise[0] = -95;
    nd->aggregate_noise = -95;
    nd->noise_unit = APPLE80211_UNIT_DBM;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getRSSI(IO80211Interface *interface, struct apple80211_rssi_data *rd) {
    rd->version = APPLE80211_VERSION;
    rd->num_radios = 1;
    rd->rssi[0] = -42;
    rd->aggregate_rssi = -42;
    rd->rssi_unit = APPLE80211_UNIT_DBM;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getTXPOWER(IO80211Interface *interface, struct apple80211_txpower_data *txd) {
    txd->version = APPLE80211_VERSION;
    txd->txpower = 100;
    txd->txpower_unit = APPLE80211_UNIT_PERCENT;
    return kIOReturnSuccess;
}

IOReturn Black80211Control::getRATE(IO80211Interface *interface, struct apple80211_rate_data *rd) {
    rd->version = APPLE80211_VERSION;
    rd->num_radios = 1;
    rd->rate[0] = 145;
    return kIOReturnSuccess;
}



