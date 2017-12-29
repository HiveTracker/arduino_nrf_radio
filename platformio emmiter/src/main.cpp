#include <Arduino.h>

#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define IS_EMMITER
#define LED 5

void blink()
{
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
  delay(500);
}

void setup() {
  pinMode(LED, OUTPUT);
  blink();
  blink();
  blink();
  clock_initialization();
  radio_configure();
}

void loop()
{
  send_packet(random(255));
  blink();
  delay(1000);
  blink();
}
