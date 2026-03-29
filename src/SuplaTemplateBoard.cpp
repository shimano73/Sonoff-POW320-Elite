/*
  Copyright (C) krycha88

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

#include "SuplaTemplateBoard.h"
#include "SuplaDeviceGUI.h"

namespace Supla {
namespace TanplateBoard {

void addTemplateBoard() {
#ifdef TEMPLATE_BOARD_JSON
#ifdef TEMPLATE_JSON
  chooseTemplateBoard(TEMPLATE_JSON);
  ConfigManager->set(KEY_BOARD, true);
#endif
#elif defined(TEMPLATE_BOARD_OLD)
  chooseTemplateBoard(ConfigESP->getDefaultTamplateBoard());
#endif
}
}  // namespace TanplateBoard
}  // namespace Supla



void addButton(uint8_t gpio, uint8_t event, uint8_t action, bool pullUp, bool invertLogic) {
  uint8_t nr = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  ConfigESP->setEvent(gpio, event);
  ConfigESP->setAction(gpio, action);
  ConfigESP->setPullUp(gpio, pullUp);
  ConfigESP->setInversed(gpio, invertLogic);

  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON);
  ConfigManager->set(KEY_MAX_BUTTON, nr + 1);
}

void addRelay(uint8_t gpio, uint8_t level) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();

  ConfigESP->setLevel(gpio, level);
  ConfigESP->setMemory(gpio, MEMORY_RESTORE);
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY);
  ConfigManager->set(KEY_MAX_RELAY, nr + 1);
}

void addLimitSwitch(uint8_t gpio) {
  uint8_t nr = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();

  ConfigESP->setGpio(gpio, nr, FUNCTION_LIMIT_SWITCH);
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, nr + 1);
}

void addLedCFG(uint8_t gpio, uint8_t level) {
  ConfigESP->setLevel(gpio, level);
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED);
}

void addLed(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_LED);
}

void addButtonCFG(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

void addRGBW(int8_t redPin, int8_t greenPin, int8_t bluePin, int8_t brightnessPin) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RGBW)->getValueInt();

  ConfigESP->setGpio(redPin, nr, FUNCTION_RGBW_RED);
  ConfigESP->setGpio(greenPin, nr, FUNCTION_RGBW_GREEN);
  ConfigESP->setGpio(bluePin, nr, FUNCTION_RGBW_BLUE);
  ConfigESP->setGpio(brightnessPin, nr, FUNCTION_RGBW_BRIGHTNESS);
  ConfigManager->set(KEY_MAX_RGBW, nr + 1);
}

void addDimmer(int8_t brightnessPin) {
  addRGBW(OFF_GPIO, OFF_GPIO, OFF_GPIO, brightnessPin);
}

void chooseTemplateBoard(uint8_t board) {
  ConfigESP->clearEEPROM();
  ConfigManager->deleteGPIODeviceValues();

  ConfigManager->set(KEY_BOARD, board);
  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");
  ConfigManager->set(KEY_CFG_MODE, CONFIG_MODE_10_ON_PRESSES);


  addLedCFG(CFG_LED_PIN, LOW);
  addButtonCFG(CFG_BUTTON_PIN);
  addButton(CFG_BUTTON_PIN, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, true, false );

  ConfigESP->setGpio(CSE7766_RX_PIN, FUNCTION_CSE7766_RX);
      
 }
