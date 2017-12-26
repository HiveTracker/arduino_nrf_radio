#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#define IS_EMMITER 0

void setup() {
  Serial.begin(115200);
  while (!Serial)
  {
    delay(500);
  }
  delay(500);
  Serial.println("HELLO");
  clock_initialization();
  radio_configure();
}

void loop()
{
  packet = millis();
  if (packet != 0)
  {
#ifdef IS_EMMITER
    send_packet();
    Serial.print("The contents of the package was ");
    Serial.println(packet);
    packet = 0;
    delay(1000);
#else
    uint32_t received = read_packet();
    Serial.print("Received packet ");
    Serial.print(received, HEX);
    Serial.print(" ");
    Serial.println(received, BIN);
#endif
  }
}

