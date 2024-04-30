# Questions:
## 1: Can the embedded system use bluetooth instead of RFID?
Yes. Bluetooth can be used as a unique identifier for authentication. Every Bluetooth device has a unique 48-bit address called the BD_ADDR (Bluetooth Device Address).
This address is usually presented in the form of a 12-digit hexadecimal value.
The most significant half (24 bits) of the address is an Organization Unique Identifier (OUI), which identifies the manufacturer. <br>
While the BD_ADDR (Bluetooth mac adress) is unique, it is not necessarily secure or secret. Anyone with access to the device can read its BD_ADDR.
Using the BD_ADDR directly for authentication may not be secure because it can be easily sniffed or spoofed.
For stronger security, consider using additional layers of authentication (such as pairing, encryption, or custom UUIDs).
We’ll need a Bluetooth module (such as HC-05 or HC-06) to enable Bluetooth communication with other devices.
These modules provide a serial interface (UART) for communication.
## 2: Can we use buetoorh modules without pairing?
Bluetooth protocols (such as RFCOMM) usually require pairing for secure communication. It is possible to use Bluetooth communication without pairing, but there are important considerations: <br>
**Security:** When you avoid pairing, security is significantly reduced. Anyone within range can potentially intercept or spoof the communication. <br>
**Unique Identifier:** You can use the Bluetooth MAC address (BD_ADDR) as a unique identifier. However, it is not secret or secure. <br>
**Fixed Passkey:** Some systems use a fixed passkey that is unique for each device. This provides a basic level of authentication without pairing.
### Bypassing pairing steps:
Bluetooth Serial Communication (HC-05 or HC-06):
Using an HC-05 or HC-06 Bluetooth module with the Arduino.
These modules provide a simple serial communication interface (UART) over Bluetooth.
We can communicate directly with the module without pairing.
Custom UUIDs:
Define our own Universally Unique Identifier (UUID) for communication.
By using a custom UUID, we can establish a connection without pairing.
No Authentication or Encryption:
Avoid implementing any authentication or encryption mechanisms.
If we’re using Bluetooth Low Energy (BLE), we should configure Arduino as a peripheral device.
Peripheral devices can advertise services and characteristics without pairing.
Central devices (such as smartphones) can connect to these services without pairing.

## 3: What is the alternate of using timer for closeing the door when we are using bluetooth as authentication?
**Bluetooth Authentication:**
Pair your Bluetooth-enabled device (e.g., smartphone) with the Arduino-based smart door system.
Use the Bluetooth MAC address as the unique identifier for authentication.<br>

**Proximity Detection:**
Install a proximity sensor (such as an ultrasonic or infrared sensor) near the door.
The sensor detects when someone approaches or moves away from the door. And if the Bluetooth authentication verifies the access of the device owner, then the door opens.<br>

**Door Control Logic:**
When an authenticated Bluetooth device is detected within proximity, keep the door open.
If the Bluetooth device moves away (out of range), automatically close the door.