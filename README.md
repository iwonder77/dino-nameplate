# Museum Touch Interactive System

A capacitive touch sensing system for the Rise of the Giants exhibit at Thanksgiving Point, featuring custom baseline tracking and signal optimization for thick dielectric overlays.

## Hardware Components

- **[Adafruit MPR121](https://www.adafruit.com/product/1982?gad_source=1&gad_campaignid=21079227318&gbraid=0AAAAADx9JvQkAsO7RS9bBB6XvtT1bf9lk&gclid=CjwKCAiAz_DIBhBJEiwAVH2XwDNIsjXkWfC8uAxjFRL1-COz-OxQyJIywEJ_eRMTO43w4skTKTorZRoCbN8QAvD_BwE)** - 12-channel capacitive touch sensor breakout
- **[SparkFun Pro Micro ESP32-C3](https://www.sparkfun.com/sparkfun-pro-micro-esp32-c3.html)** - Microcontroller with native USB and UART
- **DY-HV20T** - 20W audio playback module for triggered sound effects
- **Touch Pads** - 1.5" copper tape electrodes covered with 0.2" acrylic

## Key Features

- Enhanced sensitivity for thick dielectric materials (0.2" PLA)
- Stubborn baseline tracking to prevent drift during prolonged touches

## Software Architecture

### Custom MPR121 Class

This project extends the [Adafruit MPR121 Library](https://github.com/adafruit/Adafruit_MPR121) through inheritance to provide exhibit-specific configuration. Default baseline parameters and more were not sufficient for the installation. The original library handles all I2C communication and register management.

### Custom `begin()` Function

The overridden `begin()` method configures the MPR121 for optimal performance with thick overlays:

- **Baseline Tracking**: NCLF set to 255 samples (2.55s) to prevent baseline from tracking during sustained touches
- **Recovery Speed**: Fast baseline recovery (NCLR=4) after touch release

## Wiring

```
MPR121 -> ESP32-C3
  SDA  ->  GPIO1
  SCL  ->  GPIO2
  VCC  ->  3.3V
  GND  ->  GND

DY-HV20T -> ESP32-C3
  TX   ->  GPIO20 (RXI)
  RX   ->  GPIO21 (TXO)
  VCC  ->  5V
  GND  ->  GND
```

## Usage

```cpp
#include "MyMPR121.h"

MyMPR121 mpr121;

void setup() {
  Wire.begin();

  if (!mpr121.begin(0x5A, &Wire)) {
    Serial.println("MPR121 init failed!");
    while(1);
  }

  // All Adafruit library functions available:
  // mpr121.touched()
  // mpr121.filteredData()
  // mpr121.baselineData()
}
```

## License

Original Adafruit MPR121 library: BSD License  
Custom configuration extensions: [Your License Here]
