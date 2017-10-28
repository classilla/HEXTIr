/*
    HEXTIr-SD - Texas Instruments HEX-BUS SD Mass Storage Device
    Copyright Jim Brain and RETRO Innovations, 2017

    This code is a modification of uart functions in sd2iec:
    Copyright (C) 2007-2017  Ingo Korb <ingo@akana.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License only.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    config.h: User-configurable options to simplify hardware changes and/or
              reduce the code/ram requirements of the code.
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>
#include "autoconf.h"

/* ----- Common definitions for all AVR hardware variants ------ */

#ifdef CONFIG_UART_DEBUG
#define UART0_ENABLE
#endif

#ifdef CONFIG_UART_BUF_SHIFT
#define UART0_TX_BUFFER_SHIFT CONFIG_UART_BUF_SHIFT
#endif

#ifdef CONFIG_UART_BAUDRATE
#define UART0_BAUDRATE CONFIG_UART_BAUDRATE
#endif

#define CONFIG_DEVICE_ID 100

/* Interrupt handler for system tick */
#define SYSTEM_TICK_HANDLER ISR(TIMER1_COMPA_vect)

//#ifndef TRUE
//#define FALSE                 0
//#define TRUE                  (!FALSE)
//#endif

#if CONFIG_HARDWARE_VARIANT == 1
/* ---------- Hardware configuration: HEXTIr v1 ---------- */
#  define HEX_HSK_DDR         DDRC
#  define HEX_HSK_OUT         PORTC
#  define HEX_HSK_IN          PINC
#  define HEX_HSK_PIN         _BV(PIN4)

#  define HEX_BAV_DDR         DDRC
#  define HEX_BAV_OUT         PORTC
#  define HEX_BAV_IN          PINC
#  define HEX_BAV_PIN         _BV(PIN5)

#  define HEX_DATA_DDR        DDRC
#  define HEX_DATA_OUT        PORTC
#  define HEX_DATA_IN         PINC
#  define HEX_DATA_PIN        (_BV(PIN0) | _BV(PIN1) | _BV(PIN2) | _BV(PIN3))

#  define HAVE_SD
#  define SD_CHANGE_HANDLER     ISR(PCINT0_vect)
#  define SD_SUPPLY_VOLTAGE     (1L<<21)

/* 250kHz slow, 2MHz fast */
#  define SPI_DIVISOR_SLOW 64
#  define SPI_DIVISOR_FAST 8

static inline void sdcard_interface_init(void) {
  DDRB  &= ~_BV(PB0);  // wp
  PORTB |=  _BV(PB0);
  DDRB  &= ~_BV(PB1);  // detect
  PORTB |=  _BV(PB1);
  PCICR |= _BV(PCIE0);
  //EICRB |=  _BV(ISC60);
  PCMSK0 |= _BV(PCINT0);
  //EIMSK |=  _BV(INT6);
}

static inline uint8_t sdcard_detect(void) {
  return !(PINB & _BV(PB1));
}

static inline uint8_t sdcard_wp(void) {
  return PINB & _BV(PB0);
}

#elif CONFIG_HARDWARE_VARIANT == 2
/* ---------- Hardware configuration: uIEC v3 ---------- */
#  define HAVE_SD
#  define SD_CHANGE_HANDLER     ISR(INT6_vect)
#  define SD_SUPPLY_VOLTAGE     (1L<<21)

/* 250kHz slow, 2MHz fast */
#  define SPI_DIVISOR_SLOW 32
#  define SPI_DIVISOR_FAST 4

static inline void sdcard_interface_init(void) {
  DDRE  &= ~_BV(PE6);
  PORTE |=  _BV(PE6);
  DDRE  &= ~_BV(PE2);
  PORTE |=  _BV(PE2);
  EICRB |=  _BV(ISC60);
  EIMSK |=  _BV(INT6);
}

static inline uint8_t sdcard_detect(void) {
  return !(PINE & _BV(PE6));
}

static inline uint8_t sdcard_wp(void) {
  return PINE & _BV(PE2);
}

#else
#  error "CONFIG_HARDWARE_VARIANT is unset or set to an unknown value."
#endif


/* ---------------- End of user-configurable options ---------------- */

/* An interrupt for detecting card changes implies hotplugging capability */
#if defined(SD_CHANGE_HANDLER) || defined (CF_CHANGE_HANDLER)
#  define HAVE_HOTPLUG
#endif

/* ----- Translate CONFIG_ADD symbols to HAVE symbols ----- */
/* By using two symbols for this purpose it's easier to determine if */
/* support was enabled by default or added in the config file.       */
#if defined(CONFIG_ADD_SD) && !defined(HAVE_SD)
#  define HAVE_SD
#endif

/* Hardcoded maximum - reducing this won't save any ram */
#define MAX_DRIVES 8

/* SD access LED dummy */
#ifndef HAVE_SD_LED
# define set_sd_led(x) do {} while (0)
#endif

#endif /*CONFIG_H*/
