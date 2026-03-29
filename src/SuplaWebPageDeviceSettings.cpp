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

#include "SuplaWebPageDeviceSettings.h"

void createWebPageDeviceSettings() {
  WebServer->httpServer->on(getURL(PATH_DEVICE_SETTINGS), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleDeviceSettings();
  });
}

void handleDeviceSettings() {
  WebServer->sendHeaderStart();
  addFormHeader(S_DEVICE_SETTINGS);

  addButton(S_RELAYS, PATH_RELAY);
  addButton(S_BUTTONS, PATH_CONTROL);
  addButton(S_SENSORS_ENERGY_METER , PATH_OTHER);
  addButton(S_CONFIGURATION, PATH_CONFIG);

  addFormHeaderEnd();
  addButton(S_RETURN, "");

  WebServer->sendHeaderEnd();
}