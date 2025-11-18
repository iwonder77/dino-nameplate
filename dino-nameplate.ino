#include <Wire.h>
#include "MyMPR121.h"
#include "RingWindow.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#define RXI_PIN 20  // ESP32C3 RXI -> DY-HV20T TX
#define TXO_PIN 21  // ESP32C3 TXO -> DY-HV20T RX

const uint8_t NUM_ELECTRODES = 1;
const uint8_t MPR121_I2C_ADDR = 0x5A;

uint16_t lastTouched = 0;
uint16_t currTouched = 0;

MyMPR121 mpr121 = MyMPR121();

void playTrack(int trackNumber) {
  Serial.print("Playing track: ");
  Serial.println(trackNumber);
  // Send play command to DY-HV20T
  // Format: 0xAA 0x07 0x02 0x00 [track] [checksum]
  Serial1.write((uint8_t)0xAA);         // Start byte
  Serial1.write((uint8_t)0x07);         // Play by index command
  Serial1.write((uint8_t)0x02);         // Length high byte
  Serial1.write((uint8_t)0x00);         // Length low byte
  Serial1.write((uint8_t)trackNumber);  // Track number (1-based)

  // Calculate and send checksum (sum of all bytes & 0xFF)
  uint8_t checksum = (0xAA + 0x07 + 0x02 + 0x00 + trackNumber) & 0xFF;
  Serial1.write(checksum);

  delay(100);  // Brief delay after command
}

void setVolume(uint8_t volume) {
  // Volume range: 0-30 (0=muted, 30=max)
  if (volume > 30) volume = 30;

  Serial.print("🔊 Setting volume to: ");
  Serial.println(volume);

  // Send ASCII volume command
  Serial1.print("VOL ");
  Serial1.print(volume);
  Serial1.print("\r\n");

  delay(100);
}

void stopPlayback() {
  Serial.println("⏹ Stopping playback");

  // Stop command: 0xAA 0x04 0x00 [checksum]
  Serial1.write((uint8_t)0xAA);
  Serial1.write((uint8_t)0x04);
  Serial1.write((uint8_t)0x00);
  Serial1.write((uint8_t)((0xAA + 0x04 + 0x00) & 0xFF));

  delay(100);
}


void setup() {
  Wire.begin();
  delay(100);
  Serial.begin(9600);
  delay(10);
  Serial1.begin(9600, SERIAL_8N1, RXI_PIN, TXO_PIN);
  while (!Serial);
  delay(3000);

  if (!mpr121.begin(MPR121_I2C_ADDR, &Wire)) {
    Serial.print("MPR121 not found, check wiring");
    // hold indefinitely
    while (1);
  }

  // Serial.println("Playing sound...");
  // playTrack(1);

  Serial.println("Initial CDC and CDT Values:");
  mpr121.dumpCDCandCDTRegisters();
  delay(1000);

  mpr121.setAutoconfig(true);

  Serial.println("New CDC and CDT Values:");
  mpr121.dumpCDCandCDTRegisters();
  delay(1000);

  Serial.println("=== ELECTRODE READINGS ===");
  Serial.println("Electrode, Filtered, Baseline, Delta");
}

void loop() {
  /*
  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {
    mpr121.dumpCapData(i);
  }
  */

  // Get the currently touched pads
  currTouched = mpr121.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currTouched & _BV(i)) && !(lastTouched & _BV(i))) {
      Serial.print(i);
      Serial.println(" touched");
      delay(10);
      playTrack(1);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currTouched & _BV(i)) && (lastTouched & _BV(i))) {
      Serial.print(i);
      Serial.println(" released");
    }
  }

  // reset our state
  lastTouched = currTouched;

  // put a delay so it isn't overwhelming
  delay(100);

  Serial.println();
  delay(50);
}
