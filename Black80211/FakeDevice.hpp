//
//  FakeDevice.hpp
//  Black80211
//
//  Created by Roman Peshkov on 06/05/2018.
//  Copyright © 2018 Roman Peshkov. All rights reserved.
//

#ifndef FakeDevice_hpp
#define FakeDevice_hpp

#include <libkern/OSTypes.h>

class FakeDevice {
public:
    FakeDevice();
    ~FakeDevice();
    
    UInt32 powerState() { return m_powerState; }
    void setPowerState(UInt32 powerState) { m_powerState = powerState; };
    
private:
    UInt32 m_powerState;
};

#endif /* FakeDevice_hpp */
