//
//  Black80211Control_ioctl.cpp
//  Black80211
//
//  Created by Roman Peshkov on 30/06/2018.
//  Copyright © 2018 Roman Peshkov. All rights reserved.
//

#include "Black80211Control.hpp"

//
// MARK: 1 - SSID
//

IOReturn Black80211Control::getSSID(IO80211Interface *interface, struct apple80211_ssid_data *sd) {
    memset(sd, 0, sizeof(*sd));
    sd->version = APPLE80211_VERSION;
    strncpy((char*)sd->ssid_bytes, "anetwork", sizeof(sd->ssid_bytes));
    sd->ssid_len = (UInt32)strlen("anetwork");
    return kIOReturnSuccess;
}

IOReturn Black80211Control::setSSID(IO80211Interface *interface, struct apple80211_ssid_data *sd) {
    return kIOReturnSuccess;
}

//
// MARK: 2 - AUTH_TYPE
//

IOReturn Black80211Control::getAUTH_TYPE(IO80211Interface *interface, struct apple80211_authtype_data *ad) {
    ad->version = APPLE80211_VERSION;
    ad->authtype_lower = APPLE80211_AUTHTYPE_OPEN;    //    open at this moment
    ad->authtype_upper = APPLE80211_AUTHTYPE_NONE;    //    NO upper AUTHTYPE
    return 0;
}

//
// MARK: 4 - CHANNEL
//

IOReturn Black80211Control::getCHANNEL(IO80211Interface *interface, struct apple80211_channel_data *cd)
{
    //    IOLog("getCHANNEL c:%d f:%d\n",cd->channel.channel,cd->channel.flags);
    cd->version = APPLE80211_VERSION;
    cd->channel.version = APPLE80211_VERSION;
    cd->channel.channel = 1;
    cd->channel.flags = APPLE80211_C_FLAG_2GHZ;
    return kIOReturnSuccess;
}

//
// MARK: 7 - TXPOWER
//

IOReturn Black80211Control::getTXPOWER(IO80211Interface *interface, struct apple80211_txpower_data *txd) {
    txd->version = APPLE80211_VERSION;
    txd->txpower = 100;
    txd->txpower_unit = APPLE80211_UNIT_PERCENT;
    return kIOReturnSuccess;
}

//
// MARK: 8 - RATE
//

IOReturn Black80211Control::getRATE(IO80211Interface *interface, struct apple80211_rate_data *rd) {
    rd->version = APPLE80211_VERSION;
    rd->num_radios = 1;
    rd->rate[0] = 145;
    return kIOReturnSuccess;
}

//
// MARK: 9 - BSSID
//

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

//
// MARK: 10 - SCAN_REQ
//
IOReturn Black80211Control::setSCAN_REQ(IO80211Interface *interface, struct apple80211_scan_data *sd) {
    dev->setState(APPLE80211_S_SCAN);
    IOLog("Black80211. Scan requested. Type: %d\n", sd->scan_type);
    
//    if (interface) {
//        interface->postMessage(APPLE80211_M_SCAN_DONE);
//    }
    
    dev->setState(APPLE80211_S_INIT);
    
    return kIOReturnSuccess;
}

//
// MARK: 11 - SCAN_RESULT
//
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

//
// MARK: 12 - CARD_CAPABILITIES
//

IOReturn Black80211Control::getCARD_CAPABILITIES(IO80211Interface *interface, struct apple80211_capability_data *cd) {
    if (!cd) {
        return kIOReturnSuccess;
    }
    cd->version = APPLE80211_VERSION;
    cd->capabilities[0] = 0xab;
    cd->capabilities[1] = 0x7e;
    return kIOReturnSuccess;
}

//
// MARK: 13 - STATE
//

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

//
// MARK: 14 - PHY_MODE
//

IOReturn Black80211Control::getPHY_MODE(IO80211Interface *interface, struct apple80211_phymode_data *pd) {
    if (!pd) {
        return kIOReturnSuccess;
    }
    pd->version = APPLE80211_VERSION;
    pd->phy_mode = APPLE80211_MODE_11A | APPLE80211_MODE_11B | APPLE80211_MODE_11G;
    pd->active_phy_mode = APPLE80211_MODE_11B;
    return kIOReturnSuccess;
}

//
// MARK: 15 - OP_MODE
//

IOReturn Black80211Control::getOP_MODE(IO80211Interface *interface, struct apple80211_opmode_data *od) {
    if (!od) {
        return kIOReturnSuccess;
    }
    od->version = APPLE80211_VERSION;
    od->op_mode = APPLE80211_M_STA;
    return kIOReturnSuccess;
}

//
// MARK: 16 - RSSI
//

IOReturn Black80211Control::getRSSI(IO80211Interface *interface, struct apple80211_rssi_data *rd) {
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

IOReturn Black80211Control::getNOISE(IO80211Interface *interface,struct apple80211_noise_data *nd) {
    nd->version = APPLE80211_VERSION;
    nd->num_radios = 1;
    nd->noise[0] = -95;
    nd->aggregate_noise = -95;
    nd->noise_unit = APPLE80211_UNIT_DBM;
    return kIOReturnSuccess;
}

//
// MARK: 19 - POWER
//

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

//
// MARK: 20 - ASSOCIATE
//

IOReturn Black80211Control::setASSOCIATE(IO80211Interface *interface,struct apple80211_assoc_data *ad)
{
    IOLog("setASSOCIATE \n");
    
    //    if (interface)
    //        interface->postMessage(APPLE80211_IOC_SCAN_RESULT);
    return 0;
}

//
// MARK: 27 - SUPPORTED_CHANNELS
//

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

//
// MARK: 28 - LOCALE
//

IOReturn Black80211Control::getLOCALE(IO80211Interface *interface, struct apple80211_locale_data *ld) {
    if (!ld) {
        return kIOReturnSuccess;
    }
    ld->version = APPLE80211_VERSION;
    ld->locale  = APPLE80211_LOCALE_FCC;
    
    return kIOReturnSuccess;
}

//
// MARK: 37 - TX_ANTENNA
//
IOReturn Black80211Control::getTX_ANTENNA(IO80211Interface *interface, apple80211_antenna_data *ad)
{
    ad->version = APPLE80211_VERSION;
    ad->num_radios = 1;
    ad->antenna_index[0] = 1;
    //    ad->antenna_index[1] = 1;
    //    ad->antenna_index[2] = 1;
    return kIOReturnSuccess;
}

//
// MARK: 39 - ANTENNA_DIVERSITY
//

IOReturn Black80211Control::getANTENNA_DIVERSITY(IO80211Interface *interface, apple80211_antenna_data *ad)
{
    ad->version = APPLE80211_VERSION;
    ad->num_radios = 1;
    ad->antenna_index[0] = 1;
    //    ad->antenna_index[1] = 1;
    //    ad->antenna_index[2] = 1;
    return kIOReturnSuccess;
}

//
// MARK: 43 - DRIVER_VERSION
//

IOReturn Black80211Control::getDRIVER_VERSION(IO80211Interface *interface, struct apple80211_version_data *hv) {
    if (!hv) {
        return kIOReturnSuccess;
    }
    hv->version = APPLE80211_VERSION;
    strncpy(hv->string, "Broadcom BCM43xx 1.0 (7.21.171.133.1a2)", sizeof(hv->string));
    hv->string_len = strlen("Broadcom BCM43xx 1.0 (7.21.171.133.1a2)");
    
    return kIOReturnSuccess;
}

//
// MARK: 44 - HARDWARE_VERSION
//

IOReturn Black80211Control::getHARDWARE_VERSION(IO80211Interface *interface, struct apple80211_version_data *hv) {
    if (!hv) {
        return kIOReturnSuccess;
    }
    hv->version = APPLE80211_VERSION;
    strncpy(hv->string, "Ferrum 0", sizeof(hv->string));
    hv->string_len = strlen("Ferrum 0");
    
    return kIOReturnSuccess;
}

//
// MARK: 51 - COUNTRY_CODE
//

IOReturn Black80211Control::getCOUNTRY_CODE(IO80211Interface *interface, struct apple80211_country_code_data *cd) {
    if (!cd) {
        return kIOReturnSuccess;
    }
    cd->version = APPLE80211_VERSION;
    strncpy((char*)cd->cc, "CZ", sizeof(cd->cc));
    return kIOReturnSuccess;
}
