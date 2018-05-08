//
//  FakeDevice.cpp
//  Black80211
//
//  Created by Roman Peshkov on 06/05/2018.
//  Copyright © 2018 Roman Peshkov. All rights reserved.
//

#include "FakeDevice.hpp"
#include "apple80211.h"

FakeDevice::FakeDevice()
    : m_powerState(APPLE80211_POWER_OFF)
    , m_state(APPLE80211_S_INIT) {
    
}

FakeDevice::~FakeDevice() {
    
}
