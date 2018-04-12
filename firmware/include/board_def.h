/**
 * @file board_def.h
 * @brief TODO.
 *
 */

#ifndef BOARD_DEF_H
#define BOARD_DEF_H

#include <stdint.h>

#define OFF (0)
#define ON  (1)

#define FALSE (0)
#define TRUE  (1)

#define HIGH (1)
#define LOW (0)

#define MAX(a, b) ( (a)>(b) ? (a) : (b) )
#define MIN(a, b) ( (a)<(b) ? (a) : (b) )

#define BIT(x) (0x01 << (x))

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))

#define CPU_16MHZ (0x00)

#define cpu_prescale(n) {CLKPR = 0x80; CLKPR = (n);}

#define hard_reset() {WDTCR |= 1<<WDE;  while(1);}

#define enable_interrupt() {asm ("sei"::);}
#define disable_interrupt() {asm ("cli"::);}

#define LED_PIN PIND
#define LED_DDR DDRD
#define LED_PORT PORTD
#define LED_BIT (6)

#define PWM_PIN PINC
#define PWM_DDR DDRC
#define PWM_PORT PORTC
#define PWM_BIT (4)

#define ADC_PIN PINF
#define ADC_DDR DDRF
#define ADC_PORT PORTF

#define BUTTON_PIN PIND
#define BUTTON_DDR DDRD
#define BUTTON_PORT PORTD
#define BUTTON_BIT_BT0 (4)
#define BUTTON_BIT_BT1 (5)
#define BUTTON_BIT_BT2 (7)

#define SWITCH_PIN PINE
#define SWITCH_DDR DDRE
#define SWITCH_PORT PORTE
#define SWITCH_BIT_SW0 (0)

#define POT_PIN PINF
#define POT_DDR DDRF
#define POT_PORT PORTF
#define POT_BIT_PT0 (0)
#define POT_BIT_PT1 (1)

#define DRIVER_IO_PIN PINB
#define DRIVER_IO_DDR DDRB
#define DRIVER_IO_PORT PORTB
#define DRIVER_IO_BIT_IN1 (0)
#define DRIVER_IO_BIT_IN2 (1)
#define DRIVER_AIN_BIT_CS (2)

// TODO - move to led.h
#define led_init() bit_set(LED_DDR, BIT(LED_BIT))
#define led_on() bit_set(LED_PORT, BIT(LED_BIT))
#define led_off() bit_clear(LED_PORT, BIT(LED_BIT))
#define led_toggle() bit_flip(LED_PORT, BIT(LED_BIT))

#endif /* BOARD_H */
