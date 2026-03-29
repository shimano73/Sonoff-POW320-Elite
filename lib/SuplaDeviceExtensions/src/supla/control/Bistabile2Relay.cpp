
/*
 edition shimano73 dariuszjszymanski@gmail.com

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

#include "Bistable2Relay.h"
#include <Arduino.h>

BistableRelay::BistableRelay(int setPin, int resetPin, unsigned long pulseTime)
    : setPin(setPin),
      resetPin(resetPin),
      pulseTime(pulseTime),
      pulseStart(0),
      pulseActive(false),
      activePin(-1) {

    setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
    setDefaultStateRestore();
}

void BistableRelay::onInit() {
    pinMode(setPin, OUTPUT);
    pinMode(resetPin, OUTPUT);

    digitalWrite(setPin, LOW);
    digitalWrite(resetPin, LOW);
}

void BistableRelay::turnOn(_supla_int_t duration) {
    startPulse(setPin);
   // VirtualRelay::turnOn(duration);
    getChannel()->setNewValue(true); 
}

void BistableRelay::turnOff(_supla_int_t duration) {
    startPulse(resetPin);
    //VirtualRelay::turnOff(duration);
    getChannel()->setNewValue(false);
}
/*
void BistableRelay::toggle() {
  if isOn() turnOff();
  else turnOn();
}
 */ 
void BistableRelay::startPulse(int pin) {
    pinMode(pin, OUTPUT); // zabezpieczenie gdyby ktoś zmienił tryb
    digitalWrite(pin, HIGH);

    activePin = pin;
    pulseStart = millis();
    pulseActive = true;
}

void BistableRelay::iterateAlways() {
    if (pulseActive && (millis() - pulseStart >= pulseTime)) {
        digitalWrite(activePin, LOW);
        pulseActive = false;
        activePin = -1;
    }
}