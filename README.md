## Context

The Raspberry Pi exposes only one hardware UART, which is insufficient when simultaneous communication with multiple devices is required. In this project, each drone in a six-drone swarm uses a dedicated 433 MHz radio transceiver connected over UART. To handle six transceivers from a single Raspberry Pi, a custom expansion board (HAT) was designed using an I²C-to-UART bridge chip natively supported by Linux.

A key motivation for moving away from USB was power delivery: the Raspberry Pi's USB ports do not supply enough current to reliably drive multiple radio modules, even through powered hubs. Bypassing USB in favour of direct UART with battery-sourced power resolves this limitation.

---

## System Architecture

```
[ Battery ]
     │
     ├──► [ Voltage Regulator (5V) ] ──► [ I²C-to-UART chip ]
     │                                          │
[ Raspberry Pi ]                         UART 1 ──► Radio transceiver 1
  I²C bus ──────────────────────────►   UART 2 ──► Radio transceiver 2
                                         ...
                                         UART N ──► Radio transceiver N
```

- Power for the bridge chip is drawn **directly from the battery** through a regulator, bypassing the Raspberry Pi's USB current limit.
- The I²C bus from the Raspberry Pi controls the chip; Linux automatically exposes each UART as `/dev/ttyS0`, `/dev/ttyS1`, etc.
- Multiple chips can be stacked on the same I²C bus using different addresses, scaling up to 8 UARTs or more if needed.

---

## Repository Structure

```
.
├── Code/               # Firmware / communication validation code (C)
├── Projet_Tschema/     # PCB schematic and layout files (KiCad)
└── README.md
```

---

## Hardware

| Component | Role |
|---|---|
| Raspberry Pi | Main onboard computer (Linux) |
| I²C-to-UART bridge chip | Expands one I²C bus into 1, 2, or 4 UART channels |
| 433 MHz radio transceivers | Long-range communication with drones (~400 m through buildings) |
| Voltage regulator | Supplies 5 V (≥ 1 W) to the bridge chip from the battery |
| Custom HAT PCB | Designed with KiCad; connects directly to the Raspberry Pi GPIO header |

The radio modules operate in the **433 MHz band**, each assigned a separate channel to avoid inter-drone interference. Range achieved in field tests: ~400 m through 6–7 concrete buildings.

---

## PCB Design

The expansion board is designed in **KiCad** (files in `Projet_Tschema/`). It plugs onto the Raspberry Pi GPIO header and provides:

- I²C connection to the bridge chip
- Battery-sourced regulated 5 V power rail for the chip
- UART connectors for each radio transceiver
- Provision for a second bridge chip if more than 2–4 UARTs are needed

---

## Software

The firmware (C, `Code/`) handles:

1. **UART validation** — confirms Linux correctly enumerates the bridge chip's UART ports on the Raspberry Pi.
2. **Communication protocol** — data exchange with each radio transceiver over its dedicated UART channel.

Under Linux, bridge chip UARTs appear automatically at `/dev/ttyS*` once the I²C device is detected — no custom driver required.

---

## Build

```bash
cd Code
make
```

---

## Validation Steps

1. Mount the HAT on the Raspberry Pi and power the bridge chip from the battery rail.
2. Confirm UART devices appear: `ls /dev/ttyS*`
3. Run the communication test to validate send/receive on one channel.
4. Once validated for one channel, scale to the full 6-channel configuration.

---

## Status

> Active development — PCB validation and multi-channel firmware in progress.
