 
## Context
 
The Raspberry Pi exposes only one hardware UART, which is insufficient when simultaneous communication with multiple devices is required. In this project, each drone in a six-drone swarm uses a dedicated 433 MHz radio transceiver connected over UART. To handle six transceivers from a single Raspberry Pi, a custom expansion board (HAT) was designed using the SC16IS752, natively supported by Linux.
 
A key motivation for moving away from USB was power delivery: the Raspberry Pi's USB ports do not supply enough current to reliably drive multiple radio modules, even through powered hubs. Bypassing USB in favour of direct UART with battery-sourced power resolves this limitation.
 
---
 
## System Architecture
 
```
[ Battery ]
     │
     ├──► [ Voltage Regulator (5V) ] ──► [ SC16IS752 ]
     │                                        │
[ Raspberry Pi ]                       UART A (/dev/ttySC0) ──► Radio transceiver 1
  I²C bus ──────────────────────────►  UART B (/dev/ttySC1) ──► Radio transceiver 2
                                        (stack additional chips for more channels)
```
 
- Power for the SC16IS752 is drawn **directly from the battery** through a regulator, bypassing the Raspberry Pi's USB current limit.
- The I²C bus from the Raspberry Pi controls the chip; Linux automatically exposes each UART as `/dev/ttySC0` and `/dev/ttySC1`.
- Multiple chips can be stacked on the same I²C bus using different A0/A1 addresses, scaling up to 32 UART channels.
 
---
 
## Hardware
 
| Component | Role |
|---|---|
| Raspberry Pi | Main onboard computer (Linux) |
| SC16IS752 | I²C-to-dual-UART bridge chip (2 ch UART + 8 GPIO) |
| 433 MHz radio transceivers | Long-range communication with drones (~400 m through buildings) |
| Voltage regulator | Supplies 5V (≥ 1W) to the chip from the battery |
| Custom HAT PCB | Designed with KiCad; connects directly to the Raspberry Pi GPIO header |
 
The radio modules operate in the **433 MHz band**, each assigned a separate channel to avoid inter-drone interference.
 
---
 
## Repository Structure
 
```
.
├── Code/               # Firmware / communication validation code (C)
├── Projet_Tschema/     # PCB schematic and layout files (KiCad)
└── README.md
```
 
---
 
## Setup
 
### 1. Enable I²C on the Raspberry Pi
 
```bash
sudo raspi-config
# Navigate to: Interfacing Options → I2C → Yes
```
 
### 2. Register the SC16IS752 kernel driver
 
Append to `/boot/config.txt`:
 
```
dtoverlay=sc16is752-i2c,int_pin=24,addr=0x48
# addr changes according to A0/A1 jumper setting — default is 0x48
```
 
Then reboot:
 
```bash
sudo reboot
```
 
### 3. Verify UART devices are exposed
 
```bash
ls /dev/ttySC*
# Expected: /dev/ttySC0  /dev/ttySC1
```
 
---
 
## PCB Design
 
The expansion board is designed in **KiCad** (files in `Projet_Tschema/`). It plugs onto the Raspberry Pi GPIO header and provides:
 
- I²C connection to the SC16IS752
- Battery-sourced regulated 5V power rail for the chip
- UART connectors for each radio transceiver
   
---
 
## Software
 
The firmware (`Code/`, C) handles:
 
1. **UART validation** — confirms Linux correctly enumerates `/dev/ttySC0` and `/dev/ttySC1` after the HAT is mounted.
2. **Communication protocol** — data exchange with each radio transceiver over its dedicated UART channel.
 
---
 
 
## Validation Steps
 
1. Mount the HAT and power the SC16IS752 from the battery rail.
2. Confirm UART devices appear: `ls /dev/ttySC*`
3. Run the communication test to validate send/receive on one channel.
 
---
 
## Status
 
> Active development — PCB validation and multi-channel firmware in progress.
 
