# crispy-palm-tree
This is my group's project submission for our Microcontroller Applications Module.

For our module we focused on the Microchip Technology PIC18F4550-I/P microcontroller.
The IDE we used was MPLAB X IDE.

Our project idea was based around the MAX30102 Pulse Oximeter and Heart-Rate Sensor. Our aim was to reduced the time needed for the Singapore Civil Defence Force's ambulance to response to out-patients with heart related diseases. 

How we intended to carry out our idea was through the use of AWS. (Our lecturer told us it would be better to have a content provider for our project instead of a VM.)

The implemention consists of a on-person device , base station and dispatch end.

Components & Roles
On-person unit is a wearable, internet connected, aws iot core connected thing and heart-rate monitoring device that is constantly monitoring the wearer's heart-rate and publishes a message on a MQTT topic when no pulse is detected. The components consists of a ESP32 and a MAX30102. Ideally this device would have a built in battery, we chose not to integrate one because we only needed to demonstrate the main function of our project.

Base station unit is a internet connected, aws iot core connected thing, smart door lock which allows first responders to gain entry to a premises via a rfid card without having to break down the door. The rfid uid is sent to the rfid reader/writer when the dispatcher confirms the request for assistance. The components consists of a PIC184550, ESP8266, Mifare RC522 RFID card reader/writer, LEDs for door lock status, servo motor(door lock), LCD screen and buzzer.

Dispatch end is the AWS IOT test client console. Ideally it would be it's own application but that would be out of the scope of the project. This is where dispatch would receive request for assistant messages from the on-person unit and send rfid uids to the base station unit via the MQTT sub and pub topics.

This project is pretty bare bones but has lots of room for improvement. One would be removing the PIC18F4550 but ofcourse that was required by the module.

We do not plan to continue working on this project but there is a slight chance I may revisit this but no fingers crossed.
