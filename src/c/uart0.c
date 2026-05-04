#include "gpio.h"
#include "mmio.h"
#include <stdint.h>
#include <sys/types.h>

enum {                        // for uart0 only
  DR = 0xfe000000 + 0x201000, // data reg
  RSRECR = DR + 0x04,         //???
  FR = DR + 0x18,             // flag reg
  ILPR = DR + 0x20,           // not in use
  IBRD = DR + 0x24,           // integer baud rate divisor
  FBRD = DR + 0x28,           // fractional baud rate divisor
  LCRH = DR + 0x2c,           // line control reg
  CR = DR + 0x30,             // control reg
  IFLS = DR + 0x34,           // interrupt fifo level select reg
  IMSC = DR + 0x38,           // interrupt mask set clear reg
  RIS = DR + 0x3c,            // raw intterupt status reg
  MIS = DR + 0x40,            // masked interrupt status reg
  ICR = DR + 0x44,            // interrupt clear reg
  DMACR = DR + 0x48,          // dma control reg
  ITCR = DR + 0x80,           // test control reg
  ITIP = DR + 0x84,           // integration test input reg
  ITOP = DR + 0x88,           // integration test output reg
  TDR = DR + 0x8c,            // test data reg
  UARTCLK = 48000000,
  UART_MAX_QUEUE = 16 * 1024
};

void set_baud_rate(uint32_t baud_rate) {
  // Baud rate divisor BAUDDIV = (FUARTCLK/(16 * Baud rate))
  uint32_t divider = (16 * baud_rate);
  uint32_t ibrd = (UARTCLK / divider);
  uint32_t remainder = UARTCLK % divider;
  uint32_t fbrd =
      (remainder * 64 + (divider / 2)) / divider; // idk the math gang
  mmio_write(IBRD, 0);
  mmio_write(FBRD, 0);
  mmio_write(IBRD, ibrd);
  mmio_write(FBRD, fbrd);
}

void uart0_init() {
  mmio_write(CR, 0);   // disable uart0
  mmio_write(LCRH, 0); // flush fifo kinda hacky should be clearing specific bit
                       // but i am lazy
  mmio_write(ICR, 0x7ff); // clears interrupts
  set_baud_rate(115200);
  uint32_t line_opts = (1 << 4) | (1 << 5) | (1 << 6);
  mmio_write(LCRH, line_opts);
  mmio_write(IMSC, 0); // mask interrupts (mutes)
  gpio_sel_func(14, ALTF0);
  gpio_sel_func(15, ALTF0);
  gpio_pull_ctrl(14, PULL_NONE);
  gpio_pull_ctrl(15, PULL_NONE);
  uint32_t cr_opts = (1 << 0) | (1 << 8) | (1 << 9); // enable cr with tx and rx
  mmio_write(CR, cr_opts);
}

uint8_t uart_output_queue[UART_MAX_QUEUE]; // buffer for uart output
uint32_t uart_output_queue_write = 0; // rmb this is output to write to screen
uint32_t uart_output_queue_read =
    0; // rmb this is out so controls read pointer from buffer to screen

uint32_t isOutputQueueEmpty() {
  return uart_output_queue_read == uart_output_queue_write;
}

uint32_t isRXempty() { // polling RXFE in FR
  return (mmio_read(FR) & (1 << 4));
}

uint32_t isTXfull() { // polling TXFF in FR
  return (mmio_read(FR) & (1 << 5));
}

uint8_t readByte() {
  while (isRXempty())
    ; // while RX is empty it returns 1 and keeps waiting once returns 0 jumps
      // out of loop and works
  return (uint8_t)mmio_read(DR);
}

void writeByte(uint8_t ch) {
  while (isTXfull())
    ;
  mmio_write(DR, (uint8_t)ch);
}

void loadOutputFIFO() { // loads software buffer into hardware buffer to write
  while (!isOutputQueueEmpty() && !isTXfull()) {
    writeByte(uart_output_queue[uart_output_queue_read]);
    uart_output_queue_read =
        (uart_output_queue_read + 1) &
        (UART_MAX_QUEUE - 1); // wraps around ring buffer once go above
  }
}

void writeBytetoBuf(uint8_t ch) { // write to software buffer
  uint32_t next = (uart_output_queue_write + 1) &
                  (UART_MAX_QUEUE - 1); // calculates the next position

  while (next == uart_output_queue_read) { // checks if buffer is full since
                                           // write shldnt collide with read
    loadOutputFIFO();
  }

  uart_output_queue[uart_output_queue_write] = ch;
  uart_output_queue_write = next;
}

void writeText(char *buffer) {
  while (*buffer) { // while not a null term
    if (*buffer == '\n') {
      writeBytetoBuf('\r');
    }
    writeBytetoBuf(*buffer++);
  }
}
