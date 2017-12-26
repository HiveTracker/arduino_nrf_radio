#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "radio_config.h"
#include "nrf.h"


#define NRF_SUCCESS 1
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
  Serial.println("The packet was sent");
  NRF_RADIO->EVENTS_DISABLED = 0U;
  // Disable radio
  NRF_RADIO->TASKS_DISABLE = 1U;
  while (NRF_RADIO->EVENTS_DISABLED == 0U)
  {
    // wait
  }
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
    send_packet();
    Serial.print("The contents of the package was ");
    Serial.println(packet);
    packet = 0;
  }
  delay(1000);
 // __WFE();
}
}
