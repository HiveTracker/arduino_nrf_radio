/**
   Copyright (c) 2009 - 2017, Nordic Semiconductor ASA

   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form, except as embedded into a Nordic
      Semiconductor ASA integrated circuit in a product or a software update for
      such product, must reproduce the above copyright notice, this list of
      conditions and the following disclaimer in the documentation and/or other
      materials provided with the distribution.

   3. Neither the name of Nordic Semiconductor ASA nor the names of its
      contributors may be used to endorse or promote products derived from this
      software without specific prior written permission.

   4. This software, with or without modification, must only be used with a
      Nordic Semiconductor ASA integrated circuit.

   5. Any software provided in binary form under this license must not be reverse
      engineered, decompiled, modified and/or disassembled.

   THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/** @file
  @addtogroup nrf_dev_radio_rx_example_main nrf_dev_radio_tx_example_main
  @{
*/

#include "radio_config.h"
#include "nrf.h"

/* These are set to zero as ShockBurst packets don't have corresponding fields. */
#define PACKET_S1_FIELD_SIZE      (0UL)  /**< Packet S1 field size in bits. */
#define PACKET_S0_FIELD_SIZE      (0UL)  /**< Packet S0 field size in bits. */
#define PACKET_LENGTH_FIELD_SIZE  (0UL)  /**< Packet length field size in bits. */

/**
   @brief Function for swapping/mirroring bits in a byte.

  @verbatim
   output_bit_7 = input_bit_0
   output_bit_6 = input_bit_1
             :
   output_bit_0 = input_bit_7
  @endverbatim

   @param[in] inp is the input byte to be swapped.

   @return
   Returns the swapped/mirrored input byte.
*/
static uint32_t swap_bits(uint32_t inp);

/**
   @brief Function for swapping bits in a 32 bit word for each byte individually.

   The bits are swapped as follows:
   @verbatim
   output[31:24] = input[24:31]
   output[23:16] = input[16:23]
   output[15:8]  = input[8:15]
   output[7:0]   = input[0:7]
   @endverbatim
   @param[in] input is the input word to be swapped.

   @return
   Returns the swapped input byte.
*/
static uint32_t bytewise_bitswap(uint32_t inp);

static uint32_t swap_bits(uint32_t inp)
{
  uint32_t i;
  uint32_t retval = 0;

  inp = (inp & 0x000000FFUL);

  for (i = 0; i < 8; i++)
  {
    retval |= ((inp >> i) & 0x01) << (7 - i);
  }

  return retval;
}


static uint32_t bytewise_bitswap(uint32_t inp)
{
  return (swap_bits(inp >> 24) << 24)
         | (swap_bits(inp >> 16) << 16)
         | (swap_bits(inp >> 8) << 8)
         | (swap_bits(inp));
}


/**
   @brief Function for configuring the radio to operate in ShockBurst compatible mode.

   To configure the application running on nRF24L series devices:

   @verbatim
   uint8_t tx_address[5] = { 0xC0, 0x01, 0x23, 0x45, 0x67 };
   hal_nrf_set_rf_channel(7);
   hal_nrf_set_address_width(HAL_NRF_AW_5BYTES);
   hal_nrf_set_address(HAL_NRF_TX, tx_address);
   hal_nrf_set_address(HAL_NRF_PIPE0, tx_address);
   hal_nrf_open_pipe(0, false);
   hal_nrf_set_datarate(HAL_NRF_1MBPS);
   hal_nrf_set_crc_mode(HAL_NRF_CRC_16BIT);
   hal_nrf_setup_dynamic_payload(0xFF);
   hal_nrf_enable_dynamic_payload(false);
   @endverbatim

   When transmitting packets with hal_nrf_write_tx_payload(const uint8_t *tx_pload, uint8_t length),
   match the length with PACKET_STATIC_LENGTH.
   hal_nrf_write_tx_payload(payload, PACKET_STATIC_LENGTH);

*/
void clock_initialization()
{
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
  {
    // Do nothing.
  }
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART    = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
  {
  }
}

void radio_configure()
{
  // Radio config
  set_tx_power(RADIO_TXPOWER_TXPOWER_0dBm);
  set_frequency(100);
  set_nrf_data_rate(RADIO_MODE_MODE_Nrf_1Mbit);
  NRF_RADIO->PREFIX0 =
    ((uint32_t)swap_bits(0xC3) << 24) // Prefix byte of address 3 converted to nRF24L series format
    | ((uint32_t)swap_bits(0xC2) << 16) // Prefix byte of address 2 converted to nRF24L series format
    | ((uint32_t)swap_bits(0xC1) << 8)  // Prefix byte of address 1 converted to nRF24L series format
    | ((uint32_t)swap_bits(0xC0) << 0); // Prefix byte of address 0 converted to nRF24L series format

  NRF_RADIO->PREFIX1 =
    ((uint32_t)swap_bits(0xC7) << 24) // Prefix byte of address 7 converted to nRF24L series format
    | ((uint32_t)swap_bits(0xC6) << 16) // Prefix byte of address 6 converted to nRF24L series format
    | ((uint32_t)swap_bits(0xC4) << 0); // Prefix byte of address 4 converted to nRF24L series format

  NRF_RADIO->BASE0 = bytewise_bitswap(0x01234567UL);  // Base address for prefix 0 converted to nRF24L series format
  NRF_RADIO->BASE1 = bytewise_bitswap(0x89ABCDEFUL);  // Base address for prefix 1-7 converted to nRF24L series format

  set_tx_address(0);
  set_rx_address(1);
  set_packet_config(4, 4);

  // CRC Config
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFFFUL;   // Initial value
    NRF_RADIO->CRCPOLY = 0x11021UL;  // CRC poly: x^16 + x^12^x^5 + 1
  }
  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFUL;   // Initial value
    NRF_RADIO->CRCPOLY = 0x107UL;  // CRC poly: x^8 + x^2^x^1 + 1
  }
  NRF_RADIO->PACKETPTR = (uint32_t)&packet;
}

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

void set_tx_power(int power)
{
  NRF_RADIO->TXPOWER   = (power << RADIO_TXPOWER_TXPOWER_Pos);
}

//a frequency band in the range 0 - 100. Each step is 1MHz wide, based at 2400MHz.
void set_frequency(int band)
{
  if (band >= 0 || band <= 100)
  {
    NRF_RADIO->FREQUENCY = band;  // Frequency bin 7, 2407MHz
  }
}

void set_nrf_data_rate(char mode)
{
  NRF_RADIO->MODE = (mode << RADIO_MODE_MODE_Pos);
}

void set_tx_address(int address)
{
  NRF_RADIO->TXADDRESS = address;
}

void set_rx_address(int address)
{
  NRF_RADIO->RXADDRESSES = address;
}

void set_packet_config(int address_length, int packet_lenght)
{
  // Packet configuration
  NRF_RADIO->PCNF0 = (PACKET_S1_FIELD_SIZE     << RADIO_PCNF0_S1LEN_Pos) |
                     (PACKET_S0_FIELD_SIZE     << RADIO_PCNF0_S0LEN_Pos) |
                     (PACKET_LENGTH_FIELD_SIZE << RADIO_PCNF0_LFLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

  // Packet configuration
  NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos) |
                     (RADIO_PCNF1_ENDIAN_Big       << RADIO_PCNF1_ENDIAN_Pos)  |
                     (address_length   << RADIO_PCNF1_BALEN_Pos)   |
                     (packet_lenght         << RADIO_PCNF1_STATLEN_Pos) |
                     (packet_lenght       << RADIO_PCNF1_MAXLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"
}

