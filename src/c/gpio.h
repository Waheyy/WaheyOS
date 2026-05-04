#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
enum { // gpio registers
  PERIPHERAL_BASE = 0xfe000000,
  GPFSEL0 = PERIPHERAL_BASE + 0x200000,
  GPFSEL1 = GPFSEL0 + 0x04,
  GPFSEL2 = GPFSEL0 + 0x08,
  GPFSEL3 = GPFSEL0 + 0x0c,
  GPFSEL4 = GPFSEL0 + 0x010,
  GPFSEL5 = GPFSEL0 + 0x14,
  GPSET0 = GPFSEL0 + 0x1c,
  GPSET1 = GPFSEL0 + 0x20,
  GPCLR0 = GPFSEL0 + 0x28,
  GPCLR1 = GPFSEL0 + 0x2c,
  GPLEV0 = GPFSEL0 + 0x34,
  GPLEV1 = GPFSEL0 + 0x38,
  GPEDS0 = GPFSEL0 + 0x40,
  GPEDS1 = GPFSEL0 + 0x44,
  GPREN0 = GPFSEL0 + 0x4c,
  GPREN1 = GPFSEL0 + 0x50,
  GPFEN0 = GPFSEL0 + 0x58,
  GPFEN1 = GPFSEL0 + 0x5c,
  GPHEN0 = GPFSEL0 + 0x64,
  GPHEN1 = GPFSEL0 + 0x68,
  GPLEN0 = GPFSEL0 + 0x70,
  GPLEN1 = GPFSEL0 + 0x74,
  GPAREN0 = GPFSEL0 + 0x7c,
  GPAREN1 = GPFSEL0 + 0x80,
  GPAFEN0 = GPFSEL0 + 0x88,
  GPAFEN1 = GPFSEL0 + 0x8c,
  GPIO_PUP_PDN_CNTRL_REG0 = GPFSEL0 + 0xe4,
  GPIO_PUP_PDN_CNTRL_REG1 = GPFSEL0 + 0xe8,
  GPIO_PUP_PDN_CNTRL_REG2 = GPFSEL0 + 0xec,
  GPIO_PUP_PDN_CNTRL_REG3 = GPFSEL0 + 0xf0,
  GPIO_MAX_PINS = 53
};

enum { // for selecting functions of pin
  GPIOINPUT = 0,
  GPIOOUTPUT = 1,
  ALTF0 = 4,
  ALTF1 = 5,
  ALTF2 = 6,
  ALTF3 = 7,
  ALTF4 = 3,
  ALTF5 = 2
};

enum { // setting pull state of pin
  PULL_UP = 1,
  PULL_DOWN = 2,
  PULL_NONE = 0
};

uint32_t gpio_reg_write(uint32_t pin_number, uint32_t value, uint32_t base,
                        uint32_t field_size, uint32_t max_field);

uint32_t gpio_set(uint32_t pin_number);

uint32_t gpio_clear(uint32_t pin_number);

uint32_t gpio_pull_ctrl(uint32_t pin_number, uint32_t state);

uint32_t gpio_sel_func(uint32_t pin_number, uint32_t function);

void gpioinit_output_pull_none(uint32_t pin_number);

void gpio_hilo(uint32_t pin_number, uint32_t onOroff);

#endif
