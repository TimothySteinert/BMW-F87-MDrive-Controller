# BMW-F87-MDrive-Controller

📘 BMW MDrive Controller – V1

**Note this is not fit for use yet, almost 0 testing has been done, it cannot control any functions of the car. It purely allows for population of the M Profiles (1 and 2) in the idrive, along with editing and reset functions.**

V1 notes:
This version is focused purely around the UI aspect of the project, focusing on the Steering, power, M1 and M2 buttons, M dynamic display, and M1/M2 profile storage, edit, and reset function.

See my build thread and F-series progress post:
[F87 MDrive Integration via CAN](https://f87.bimmerpost.com/forums/showthread.php?t=2170233)
Retrofitted:
M1/M2 buttons on Steering wheel
M2c Centre console buttons
M4 GWS
M2c Drivelogic buttons
M2c Kombi

Current issues:
Poor detection of vehicle awake/ignition status. 
-Currently detects 0x12F packets and uses those to trigger an "ignition Status" state. This is a problem as the device waits to timeout and keeps sending alive packets on 0x42E which results in the vehicle stayin alive and it never sleeps.
-Poor detection of ignition status. Currently checks for any 0x12F message regardless of content, has no function to check for an awake status and ignition status
-M profiles currently just stored in a memory buffer, once the device power cycles profiles will be reset to default and not save

To Do:
Finish decoding 0x42F, 0x42E, and 0xDE
Find CAN ID and decode message for M1 and M2 buttons
Enable sending 0xDE via PTCAN to Kombi
Possibly move logic entirely to PTCAN and rely on the FEM_GW to forward messages between the KCAN2 and PTCAN like it does in an S55 DME.
Resolve ignition/awake detection
Find CAN ID responsible for HU "Driver Profile" To allow storage of M1 and M2 for all driver profiles
Change to EEPROM based storage for perminant retention of M1 and M2 profiles


This module emulates BMW MDrive functionality over K-CAN2, allowing iDrive (HU_NBT) to display and configure M1/M2 driving profiles even in vehicles that lack a factory M DME. It intercepts 0x42F messages from the head unit and responds with 0x42E status/configuration messages, replicating DME behavior.

Designed for retrofits, coding, or custom cluster integrations in F-series BMWs such as the F87, F30, F10, etc.

🛠️ Features
Detects ignition state via 0x12F CAN message
Handles incoming 0x42F MDrive profile messages from HU
Emulates 0x42E DME responses to populate MDrive iDrive UI
Dynamic updates for M1 and M2 profiles
Reset support for profile defaults


🧰 Components Used
ESP32
MCP2515	CAN controller with TJA1050 transceiver
Jumper Wires

⚡ Pinout
ESP32 Pin	MCP2515 Pin	Description
GPIO 5	CS	SPI Chip Select
GPIO 18	SCK	SPI Clock
GPIO 23	MOSI	SPI MOSI
GPIO 19	MISO	SPI MISO
GPIO 2	INT	MCP Interrupt
5V	VCC	Power
GND	GND	Ground

🚗 Vehicle Connection (K-CAN2)
Vehicle Wire	MCP2515
KCAN2 High	CAN_H
KCAN2 Low	CAN_L
**Connected to KCAN2 via the ZBE

🔗 References & Credits
🙏 Credit to superwofy's E9X M CAN Integration Module
Much of the logic around MDrive message handling, profile parsing, and CAN communication was inspired by this excellent open-source project for the E9X platform.
https://github.com/superwofy/E9X-M-CAN-Integration-Module

