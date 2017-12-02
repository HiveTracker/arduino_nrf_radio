#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "radio_config.h"
#include "nrf.h"

#define NRF_SUCCESS 1
static uint32_t packet;                    /**< Packet to transmit. */


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
  /* Start 16 MHz crystal oscillator */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;

  /* Wait for the external oscillator to start up */
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
  {
    // Do nothing.
  }

  /* Start low frequency crystal oscillator for app_timer(used by bsp)*/
  NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART    = 1;

  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
  {
    // Do nothing.
  }
}



void setup() {
Serial.begin(9600);
  uint32_t err_code = NRF_SUCCESS;

  clock_initialization();







  // Set radio configuration parameters
  radio_configure();

  // Set payload pointer
  NRF_RADIO->PACKETPTR = (uint32_t)&packet;


}

void loop() {
  if (packet != 0)
  {
    send_packet();
    Serial.println("The contents of the package was %u");//, (unsigned int)packet));
    packet = 0;
  }

  __WFE();
}
