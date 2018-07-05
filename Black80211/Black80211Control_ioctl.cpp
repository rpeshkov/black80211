//
//  Black80211Control_ioctl.cpp
//  Black80211
//
//  Created by Roman Peshkov on 30/06/2018.
//  Copyright © 2018 Roman Peshkov. All rights reserved.
//

#include "Black80211Control.hpp"

const char *fake_ssid = "UPC5424297";
const uint8_t fake_bssid[] = { 0x64, 0x7C, 0x34, 0x5C, 0x1C, 0x40 };
const char *fake_hw_version = "Hardware 1.0";
const char *fake_drv_version = "Driver 1.0";
const char *fake_country_code = "CZ";

const apple80211_channel fake_channel = {
    .version = APPLE80211_VERSION,
    .channel = 1,
    .flags = APPLE80211_C_FLAG_2GHZ | APPLE80211_C_FLAG_20MHZ | APPLE80211_C_FLAG_ACTIVE
};

// This string contains information elements from beacon frame that I captured via Wireshark
const char beacon_ie[] = "\x00\x0a\x55" \
"\x50\x43\x35\x34\x32\x34\x32\x39\x37\x01\x08\x82\x84\x8b\x96\x0c" \
"\x12\x18\x24\x03\x01\x01\x05\x04\x00\x01\x00\x00\x07\x06\x43\x5a" \
"\x20\x01\x0d\x14\x2a\x01\x04\x32\x04\x30\x48\x60\x6c\x2d\x1a\xad" \
"\x01\x1b\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x04\x06\xe6\xe7\x0d\x00\x3d\x16\x01\x00\x17\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x4a\x0e\x14\x00\x0a\x00\x2c\x01\xc8\x00\x14\x00\x05\x00\x19" \
"\x00\x7f\x01\x01\xdd\x18\x00\x50\xf2\x02\x01\x01\x80\x00\x03\xa4" \
"\x00\x00\x27\xa4\x00\x00\x42\x43\x5e\x00\x62\x32\x2f\x00\xdd\x09" \
"\x00\x03\x7f\x01\x01\x00\x00\xff\x7f\x30\x18\x01\x00\x00\x0f\xac" \
"\x02\x02\x00\x00\x0f\xac\x04\x00\x0f\xac\x02\x01\x00\x00\x0f\xac" \
"\x02\x00\x00\xdd\x1a\x00\x50\xf2\x01\x01\x00\x00\x50\xf2\x02\x02" \
"\x00\x00\x50\xf2\x04\x00\x50\xf2\x02\x01\x00\x00\x50\xf2\x02\xdd" \
"\x22\x00\x50\xf2\x04\x10\x4a\x00\x01\x10\x10\x44\x00\x01\x02\x10" \
"\x57\x00\x01\x01\x10\x3c\x00\x01\x01\x10\x49\x00\x06\x00\x37\x2a" \
"\x00\x01\x20";

//
// MARK: 1 - SSID
//

IOReturn Black80211Control::getSSID(IO80211Interface *interface,
                                    struct apple80211_ssid_data *sd) {
    return kIOReturnError;
    
    bzero(sd, sizeof(*sd));
    sd->version = APPLE80211_VERSION;
    strncpy((char*)sd->ssid_bytes, fake_ssid, sizeof(sd->ssid_bytes));
    sd->ssid_len = (uint32_t)strlen(fake_ssid);
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setSSID(IO80211Interface *interface,
                                    struct apple80211_ssid_data *sd) {
    return kIOReturnSuccess;
}

//
// MARK: 2 - AUTH_TYPE
//

IOReturn Black80211Control::getAUTH_TYPE(IO80211Interface *interface,
                                         struct apple80211_authtype_data *ad) {
    ad->version = APPLE80211_VERSION;
    ad->authtype_lower = APPLE80211_AUTHTYPE_OPEN;
    ad->authtype_upper = APPLE80211_AUTHTYPE_NONE;
    return kIOReturnSuccess;
}

//
// MARK: 4 - CHANNEL
//

IOReturn Black80211Control::getCHANNEL(IO80211Interface *interface,
                                       struct apple80211_channel_data *cd) {
    return kIOReturnError;
    
    bzero(cd, sizeof(*cd));
    
    cd->version = APPLE80211_VERSION;
    cd->channel = fake_channel;
    return kIOReturnSuccess;
}

//
// MARK: 7 - TXPOWER
//

IOReturn Black80211Control::getTXPOWER(IO80211Interface *interface,
                                       struct apple80211_txpower_data *txd) {
    return kIOReturnError;

    txd->version = APPLE80211_VERSION;
    txd->txpower = 100;
    txd->txpower_unit = APPLE80211_UNIT_PERCENT;
    return kIOReturnSuccess;
}

//
// MARK: 8 - RATE
//

IOReturn Black80211Control::getRATE(IO80211Interface *interface, struct apple80211_rate_data *rd) {
    return kIOReturnError;

    rd->version = APPLE80211_VERSION;
    rd->num_radios = 1;
    rd->rate[0] = 54;
    return kIOReturnSuccess;
}

//
// MARK: 9 - BSSID
//

IOReturn Black80211Control::getBSSID(IO80211Interface *interface,
                                     struct apple80211_bssid_data *bd) {
    return kIOReturnError;
    
    bzero(bd, sizeof(*bd));
    
    bd->version = APPLE80211_VERSION;
    memcpy(bd->bssid.octet, fake_bssid, sizeof(fake_bssid));
    return kIOReturnSuccess;
}

static IOReturn scanAction(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3) {
    IOSleep(200);
    IO80211Interface *iface = (IO80211Interface *)arg0;
    FakeDevice *dev = (FakeDevice*)arg1;
    iface->postMessage(APPLE80211_M_SCAN_DONE);
    return kIOReturnSuccess;
}

//
// MARK: 10 - SCAN_REQ
//
IOReturn Black80211Control::setSCAN_REQ(IO80211Interface *interface,
                                        struct apple80211_scan_data *sd) {
    if (dev->state() == APPLE80211_S_SCAN) {
        return kIOReturnBusy;
    }
    dev->setState(APPLE80211_S_SCAN);
    IOLog("Black80211. Scan requested. Type: %u\n"
          "BSS Type: %u\n"
          "PHY Mode: %u\n"
          "Dwell time: %u\n"
          "Rest time: %u\n"
          "Num channels: %u\n",
          sd->scan_type,
          sd->bss_type,
          sd->phy_mode,
          sd->dwell_time,
          sd->rest_time,
          sd->num_channels);
    
    if (interface) {
        dev->setPublished(false);
        fCommandGate->runAction(scanAction, interface, dev);
    }
    
    return kIOReturnSuccess;
}

//
// MARK: 11 - SCAN_RESULT
//
IOReturn Black80211Control::getSCAN_RESULT(IO80211Interface *interface,
                                           struct apple80211_scan_result **sr) {
    if (dev->published()) {
        dev->setState(APPLE80211_S_INIT);
        return 0xe0820446;
    }
    
    struct apple80211_scan_result* result =
        (struct apple80211_scan_result*)IOMalloc(sizeof(struct apple80211_scan_result));
    
    
    
    bzero(result, sizeof(*result));
    result->version = APPLE80211_VERSION;
    
    result->asr_channel = fake_channel;
    
    result->asr_noise = -101;
//    result->asr_snr = 60;
    result->asr_rssi = -73;
    result->asr_beacon_int = 100;
    
    result->asr_cap = 0x411;
    
    result->asr_age = 0;
    
    memcpy(result->asr_bssid, fake_bssid, sizeof(fake_bssid));
    
    result->asr_nrates = 1;
    result->asr_rates[0] = 54;
    
    strncpy((char*)result->asr_ssid, fake_ssid, sizeof(result->asr_ssid));
    result->asr_ssid_len = strlen(fake_ssid);
    
    result->asr_ie_len = 246;
    result->asr_ie_data = IOMalloc(result->asr_ie_len);
    memcpy(result->asr_ie_data, beacon_ie, result->asr_ie_len);

    *sr = result;
    
    dev->setPublished(true);
    
    return kIOReturnSuccess;
}

//
// MARK: 12 - CARD_CAPABILITIES
//

IOReturn Black80211Control::getCARD_CAPABILITIES(IO80211Interface *interface,
                                                 struct apple80211_capability_data *cd) {
    cd->version = APPLE80211_VERSION;
    cd->capabilities[0] = 0xab;
    cd->capabilities[1] = 0x7e;
    return kIOReturnSuccess;
}

//
// MARK: 13 - STATE
//

IOReturn Black80211Control::getSTATE(IO80211Interface *interface,
                                     struct apple80211_state_data *sd) {
    sd->version = APPLE80211_VERSION;
    sd->state = dev->state();
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setSTATE(IO80211Interface *interface,
                                     struct apple80211_state_data *sd) {
    IOLog("Black82011: Setting state: %u", sd->state);
    dev->setState(sd->state);
    return kIOReturnSuccess;
}

//
// MARK: 14 - PHY_MODE
//

IOReturn Black80211Control::getPHY_MODE(IO80211Interface *interface,
                                        struct apple80211_phymode_data *pd) {
    pd->version = APPLE80211_VERSION;
    pd->phy_mode = APPLE80211_MODE_11A
                 | APPLE80211_MODE_11B
                 | APPLE80211_MODE_11G;
    pd->active_phy_mode = APPLE80211_MODE_AUTO;
    return kIOReturnSuccess;
}

//
// MARK: 15 - OP_MODE
//

IOReturn Black80211Control::getOP_MODE(IO80211Interface *interface,
                                       struct apple80211_opmode_data *od) {
    od->version = APPLE80211_VERSION;
    od->op_mode = APPLE80211_M_STA;
    return kIOReturnSuccess;
}

//
// MARK: 16 - RSSI
//

IOReturn Black80211Control::getRSSI(IO80211Interface *interface,
                                    struct apple80211_rssi_data *rd) {
    return kIOReturnError;
    
    bzero(rd, sizeof(*rd));
    rd->version = APPLE80211_VERSION;
    rd->num_radios = 1;
    rd->rssi[0] = -42;
    rd->aggregate_rssi = -42;
    rd->rssi_unit = APPLE80211_UNIT_DBM;
    return kIOReturnSuccess;
}

//
// MARK: 17 - NOISE
//

IOReturn Black80211Control::getNOISE(IO80211Interface *interface,
                                     struct apple80211_noise_data *nd) {
    return kIOReturnError;
    
    bzero(nd, sizeof(*nd));
    nd->version = APPLE80211_VERSION;
    nd->num_radios = 1;
    nd->noise[0] = -101;
    nd->aggregate_noise = -101;
    nd->noise_unit = APPLE80211_UNIT_DBM;
    return kIOReturnSuccess;
}

//
// MARK: 18 - INT_MIT
//
IOReturn Black80211Control::getINT_MIT(IO80211Interface* interface,
                                       struct apple80211_intmit_data* imd) {
    imd->version = APPLE80211_VERSION;
    imd->int_mit = APPLE80211_INT_MIT_AUTO;
    return kIOReturnSuccess;
}


//
// MARK: 19 - POWER
//

IOReturn Black80211Control::getPOWER(IO80211Interface *interface,
                                     struct apple80211_power_data *pd) {
    pd->version = APPLE80211_VERSION;
    pd->num_radios = 1;
    pd->power_state[0] = dev->powerState();
    
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setPOWER(IO80211Interface *interface,
                                     struct apple80211_power_data *pd) {
    if (pd->num_radios > 0) {
        dev->setPowerState(pd->power_state[0]);
    }
    
    return kIOReturnSuccess;
}

//
// MARK: 20 - ASSOCIATE
//

IOReturn Black80211Control::setASSOCIATE(IO80211Interface *interface,
                                         struct apple80211_assoc_data *ad) {
    return kIOReturnError;
}

//
// MARK: 27 - SUPPORTED_CHANNELS
//

IOReturn Black80211Control::getSUPPORTED_CHANNELS(IO80211Interface *interface,
                                                  struct apple80211_sup_channel_data *ad) {
    ad->version = APPLE80211_VERSION;
    ad->num_channels = 1;
    ad->supported_channels[0] = fake_channel;
    return kIOReturnSuccess;
}

//
// MARK: 28 - LOCALE
//

IOReturn Black80211Control::getLOCALE(IO80211Interface *interface,
                                      struct apple80211_locale_data *ld) {
    ld->version = APPLE80211_VERSION;
    ld->locale  = APPLE80211_LOCALE_FCC;
    
    return kIOReturnSuccess;
}

//
// MARK: 37 - TX_ANTENNA
//
IOReturn Black80211Control::getTX_ANTENNA(IO80211Interface *interface,
                                          apple80211_antenna_data *ad) {
    ad->version = APPLE80211_VERSION;
    ad->num_radios = 1;
    ad->antenna_index[0] = 1;
    return kIOReturnSuccess;
}

//
// MARK: 39 - ANTENNA_DIVERSITY
//

IOReturn Black80211Control::getANTENNA_DIVERSITY(IO80211Interface *interface,
                                                 apple80211_antenna_data *ad) {
    ad->version = APPLE80211_VERSION;
    ad->num_radios = 1;
    ad->antenna_index[0] = 1;
    return kIOReturnSuccess;
}

//
// MARK: 43 - DRIVER_VERSION
//

IOReturn Black80211Control::getDRIVER_VERSION(IO80211Interface *interface,
                                              struct apple80211_version_data *hv) {
    hv->version = APPLE80211_VERSION;
    strncpy(hv->string, fake_drv_version, sizeof(hv->string));
    hv->string_len = strlen(fake_drv_version);
    return kIOReturnSuccess;
}

//
// MARK: 44 - HARDWARE_VERSION
//

IOReturn Black80211Control::getHARDWARE_VERSION(IO80211Interface *interface,
                                                struct apple80211_version_data *hv) {
    hv->version = APPLE80211_VERSION;
    strncpy(hv->string, fake_hw_version, sizeof(hv->string));
    hv->string_len = strlen(fake_hw_version);
    return kIOReturnSuccess;
}

//
// MARK: 51 - COUNTRY_CODE
//

IOReturn Black80211Control::getCOUNTRY_CODE(IO80211Interface *interface,
                                            struct apple80211_country_code_data *cd) {
    cd->version = APPLE80211_VERSION;
    strncpy((char*)cd->cc, fake_country_code, sizeof(cd->cc));
    return kIOReturnSuccess;
}

//
// MARK: 57 - MCS
//
IOReturn Black80211Control::getMCS(IO80211Interface* interface, struct apple80211_mcs_data* md) {
    md->version = APPLE80211_VERSION;
    md->index = APPLE80211_MCS_INDEX_AUTO;
    return kIOReturnSuccess;
}
