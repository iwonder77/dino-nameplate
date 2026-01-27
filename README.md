# Dino Nameplate

https://github.com/user-attachments/assets/d761b371-b2cf-4e69-9180-89696157810f

## Overview

The capacitive touch sensing firmware for the Supersaurus nameplate of the Rise of the Giants exhibit at Thanksgiving Point. Features a custom MPR121 class, extended from [Adafruit's MPR121 Library](https://github.com/adafruit/Adafruit_MPR121), that configures baseline tracking registers, applies ema filtering to ensure touch events are detected with thick acrylic overlays, and has additional logic that triggers video and sound playback efficiently.

## Hardware

- **[Adafruit MPR121](https://www.adafruit.com/product/1982?gad_source=1&gad_campaignid=21079227318&gbraid=0AAAAADx9JvQkAsO7RS9bBB6XvtT1bf9lk&gclid=CjwKCAiAz_DIBhBJEiwAVH2XwDNIsjXkWfC8uAxjFRL1-COz-OxQyJIywEJ_eRMTO43w4skTKTorZRoCbN8QAvD_BwE)** - 12-channel capacitive touch sensor breakout
- **ESP32-DevkitC-v4**
- **DY-HV20T** - 20W audio playback module for triggered/UART sound playback
- **Touch Pads** - 3x 1/16" aluminum plates covered with copper tape for ease of soldering (see `docs/` for more info on construction)
- **BrightSigns** - 2x BrightSigns for video playback

## Wiring Diagram

![schematic diagram](docs/supersaurus_nameplate.jpg)

## Software Architecture

### `MPR121` Class

The source code player of the match is the custom MPR121 class. As mentioned previously, it uses snippets of [Adafruit's MPR121 Library](https://github.com/adafruit/Adafruit_MPR121) (mainly the register read/write operations) but also includes:

- Enhanced filtering for touch sensing on thick 0.2" acrylic overlay
- Stubborn baseline tracking to prevent immediate drift during touch events

Key Methods:

- `begin()`: Configures the MPR121's registers for better performance with thick overlays. Mainly changing the baseline tracking and filtering parameters. For more information on each configured register, please refer to the `Touch` namespace in the `Config.h` file and the MPR121 [datasheet](https://cdn-shop.adafruit.com/datasheets/MPR121.pdf) + application notes.
- `touched()`: Applies ema filtering to individual electrode readings (filtered and baseline), and uses hysteresis thresholds + debouncing to ensure accurate detection of touch/release events in a noisy museum exhibit environment.

### `App` Class

Contains logic for hardware setup and runs a state machine in the main game loop. It also contains a debugging method which is executed when the user sets the appropriate `AppState` enum.

Key Methods:

- `run()`: Efficient state machine that considers hardware based limits such as the audio module's busy pin assertion, safety timeouts for audio module failure modes, and includes a cool down period for stabilization of system components.
