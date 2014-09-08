arduino-offgrid-solar
=====================

Arduino software and hardware to control a 12V off-grid solar system with 1000W power inverter for a fridge and lighting. The arduino is a 3V JeeNode, but I might change it for a simple arduino. The power inverter is a HQ Inverter (probably Chinese built) with a remote control input port. The arduino will turn on and off the Power inverter via the remote control port as needed to limit the energy consumption of the whole system to the power that is pre-calculated and to avoid running the inverter all the time, which would drain the battery without benefit. Additionally supply power on demand (when a 3V logic button is pressed) for lighting and radio (eventually 12V driven)

Example: Switch ON for 6 minutes every 1 hour is 240Wh with a known load of a 100W refrigerator. If it is hot the refrigerator has higher temp, but limited energy consumption per day. If it is cold the refrigerator may not use its 6 minutes to keep cold, but the inverter will drain 0.5A (amps) when on.

HACKS
=====
The biggest challenge was to make a connector for the remote port and decode its connections. It is a narrow version of a US-phone connector. So I thinned such a connector on both sides until it fit with sandpaper. The connector supplies 12V logic signals :-( and a pulsed on/off line :-( not the quick and easy thing. To down-convert, I use a voltage divider. To up convert the 3V logic to 12V (the push button switch) I use an up converter with 2 transistors NPN/PNP (see pics). 

DOCs
====
Pics and screenshots from a manual of a similar model. 

12V to 3V Logic Conversion
==========================
A simple resistive voltage divider would work best in this case. The voltage divider detects presence of 12V (HIGH @3V3 or 5V) and converts the voltage of the 12V circuit down to Arduino levels. http://en.wikipedia.org/wiki/Voltage_divider

 - Vout = R2 / (R1+R2) * Vin
 - 3300/(10000 + 3300) * 12.8V = 3.17V
 - 6800/(10000 + 6800) * 12.8V = 5.1V

Be aware that the voltage of a car may be higher than 12V, but at the lead-battery it should not exceed 13.8V for charging reasons. You could also try to use this to detect charging levels of the battery if you connect it to an Analog input (however changing reference voltages are a problem!).

WARNING:
========
Currently not all information except the software to build this system may be pushed. TODO: publish the circuits for the shield.




