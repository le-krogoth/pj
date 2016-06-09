PJ
===

Copyright (c) 2016 by Krogoth of
Ministry of Zombie Defense - http://www.mzd.org.uk/
and contributing authors

## About ##

PJ is the personal companion (frontend) to the HARDAC backend. It plays so called movies (two streams of LED blinking 
instructions) which it gets from the backend. There is the possibility to vote. You do so pushing one of the two buttons. 
PJ then sends your ballot to the HARDAC backend.

This software was written with <3 for the area41 security conference: http://area41.io/badge/


## Prerequisites ##

You will either need a conference badge, some development board (aim for the NodeMCU v2) or a naked ESP8266. If your dev 
board does not come with a USB connection, you will need a UART/TTL cable.

You can find these cables, naked ESP8266s as well as NodeMCUs for less than $5 on the Internets.

If you go the naked ESP8266 way (instead of Badge/NodeMCU), please do some reading before, for the chip can be frustrating 
at times. The mentioned dev boards take away much of that frustration potential.

If you want to flash the badge, consider to attach some pins to the connections on the left. This will help in getting
a connection which is much more stable. And the ESP8266 hates unstable connections during flashing. Power as well as RX/TX...


## Installation ##

Make sure you do have a recent Arduino IDE. Then follow these instructions (Board Manager works fine with NodeMCU, YMMV):

https://github.com/esp8266/Arduino

Open up the pj.ino file in the Arduino IDE, select your ESP8266 board, compile and flash. Make sure to change the 
ssid, pwd and server setup to your setup. You can find all of these settings in the global.h file.

Make sure to have a HARDAC instance running as well.
Please make sure that you connect RX->TX, TX->RX and reset the board (by holding the right button or whatever is needed 
on your board) and starting power. You need to pull down GPIO0 to GND on the ESP8266 to puts the chip into flashing mode.


## Licence ##

This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 See LICENSE file for details. 