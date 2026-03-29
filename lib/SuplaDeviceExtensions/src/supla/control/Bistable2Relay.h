
 // edition shimano73 dariuszjszymanski@gmail.com
 
/*
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#pragma once

#include <supla/control/virtual_relay.h>

class BistableRelay : public Supla::Control::VirtualRelay {
public:
    BistableRelay(int setPin, int resetPin, unsigned long pulseTime = 100);

    void onInit() override;
    void turnOn(_supla_int_t duration = 0) override;
    void turnOff(_supla_int_t duration = 0) override;
    //void toggle();
    void iterateAlways() override;

private:
    void startPulse(int pin);

    int setPin;
    int resetPin;

    unsigned long pulseTime;
    unsigned long pulseStart;

    bool pulseActive;
    int activePin;
};