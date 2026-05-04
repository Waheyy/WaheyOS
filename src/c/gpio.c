#include "gpio.h"
#include "mmio.h"
#include <stdint.h>

uint32_t gpio_reg_write(uint32_t pin_number, uint32_t value, uint32_t base,
                        uint32_t field_size, uint32_t max_field) {
  uint32_t field_mask = (1 << field_size) - 1;

  if (pin_number > max_field || pin_number < 0) {
    return 1;
  }
  if (value > field_mask) {
    return 1;
  }

  uint32_t num_fields =
      32 / field_size; // to find number of pins controlled in that reg
  uint32_t reg = base + ((pin_number / num_fields) *
                         4); // to find reg easily with any pin number
  uint32_t shift = (pin_number % num_fields) * field_size;

  uint32_t curval = mmio_read(reg);
  curval &= ~(field_mask << shift);
  curval |= value << shift;
  mmio_write(reg, curval);
  return 0;
}

uint32_t gpio_set(uint32_t pin_number) {
  uint32_t reg = GPSET0 + ((pin_number / 32) * 4);
  uint32_t value = 1 << pin_number;
  mmio_write(reg, value);
  return 0;
}

uint32_t gpio_clear(uint32_t pin_number) {
  uint32_t reg = GPCLR0 + ((pin_number / 32) * 4);
  uint32_t value = 1 << pin_number;
  mmio_write(reg, value);
  return 0;
}

uint32_t gpio_pull_ctrl(uint32_t pin_number, uint32_t state) {
  return gpio_reg_write(pin_number, state, GPIO_PUP_PDN_CNTRL_REG0, 2,
                        GPIO_MAX_PINS);
}

uint32_t gpio_sel_func(uint32_t pin_number, uint32_t function) {
  return gpio_reg_write(pin_number, function, GPFSEL0, 3, GPIO_MAX_PINS);
}

void gpioinit_output_pull_none(uint32_t pin_number) {
  gpio_pull_ctrl(pin_number, PULL_NONE);
  gpio_sel_func(pin_number, GPIOOUTPUT);
}

void gpio_hilo(uint32_t pin_number, uint32_t onOroff) {
  if (onOroff) {
    gpio_set(pin_number);
  } else {
    gpio_clear(pin_number);
  }
}
