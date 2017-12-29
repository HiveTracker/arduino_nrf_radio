#include <Arduino.h>

#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#define IS_EMMITER
#define LED 6



void setup() {
  Serial.begin(115200);

  delay(3000);
  Serial.print("HELLO I AM ");
  clock_initialization();
  radio_configure();
  Serial.print("RECEIVER");

}

void loop()
{
  uint32_t received = read_packet();
  Serial.print("Received packet ");
  Serial.print(received, HEX);
  Serial.print(" ");
  Serial.print(received, BIN);
  Serial.print(" ");
  Serial.println(millis());
}
