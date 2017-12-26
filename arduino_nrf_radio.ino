#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "radio_config.h"
#include "nrf.h"


#define NRF_SUCCESS 1
//#define IS_EMMITER 0
static uint32_t packet;

void send_packet()
{
  // send the packet:
  NRF_RADIO->EVENTS_READY = 0U;
  NRF_RADIO->TASKS_TXEN   = 1;

  while (NRF_RADIO->EVENTS_READY == 0U)
  {
    // wait
  }
  NRF_RADIO->EVENTS_END  = 0U;
  NRF_RADIO->TASKS_START = 1U;

  while (NRF_RADIO->EVENTS_END == 0U)
  {
    // wait
  }
  NRF_RADIO->EVENTS_DISABLED = 0U;
  // Disable radio
  NRF_RADIO->TASKS_DISABLE = 1U;
  while (NRF_RADIO->EVENTS_DISABLED == 0U)
  {
    // wait
  }
}

uint32_t read_packet()
{
  uint32_t result = 0;

  NRF_RADIO->EVENTS_READY = 0U;
  // Enable radio and wait for ready
  NRF_RADIO->TASKS_RXEN = 1U;

  while (NRF_RADIO->EVENTS_READY == 0U)
  {
  }
  NRF_RADIO->EVENTS_END = 0U;
  // Start listening and wait for address received event
  NRF_RADIO->TASKS_START = 1U;

  // Wait for end of packet or buttons state changed
  while (NRF_RADIO->EVENTS_END == 0U)
  {
    // wait
  }

  if (NRF_RADIO->CRCSTATUS == 1U)
  {
    result = packet;
  }
  NRF_RADIO->EVENTS_DISABLED = 0U;
  // Disable radio
  NRF_RADIO->TASKS_DISABLE = 1U;

  while (NRF_RADIO->EVENTS_DISABLED == 0U)
  {
    // wait
  }
  return result;
}

void clock_initialization()
{
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
  {
    // Do nothing.
  }
  NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART    = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
  {
  }
}



void setup() {
  Serial.begin(9600);
  while (!Serial)
  {
    delay(500);
  }
  delay(500);
  uint32_t err_code = NRF_SUCCESS;
  clock_initialization();
  // Set radio configuration parameters
  radio_configure();
  // Set payload pointer
  NRF_RADIO->PACKETPTR = (uint32_t)&packet;
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
