#include "mmio.h"
#include <stdint.h>
enum {
  VIDEOCORE_MBOX = 0xfe000000 + 0x0000b880,
  MBOX_READ = VIDEOCORE_MBOX,
  MBOX_POLL = VIDEOCORE_MBOX + 0x10,
  MBOX_SENDER = VIDEOCORE_MBOX + 0x14,
  MBOX_STATUS =
      VIDEOCORE_MBOX +
      0x18, // bit 30 says if read is empty and bit 31 says if write is empty
  MBOX_CONFIG = VIDEOCORE_MBOX + 0x1c,
  MBOX_WRITE = VIDEOCORE_MBOX + 0x20,
  MBOX_RESPONSE_SUCCESS = 0x80000000,
  MBOX_RESPONSE_ERR = 0x80000001,
  MBOX_FULL = 0x80000000, // 31 bit set checks for if write is full
  MBOX_EMPTY = 0x40000000 // 30 bit set checks if read fifo is empty

};

// must make sure that it is 16 byte aligned if not cant put the channels
volatile uint32_t __attribute__((
    aligned(0x10))) mbox_msg[36]; // size of the buffer for the msg

uint32_t mbox_call(uint8_t channel) {
  // set the 28 bit buffer addr and channel
  // clears last 4 bits of buffer addr and then add the channel to it
  uint32_t request = ((uint32_t)((long)&mbox_msg) & ~0xf) | (channel & 0xf);

  // waits til can write
  while (mmio_read(MBOX_STATUS) & MBOX_FULL)
    ; // returns 1 when its full

  mmio_write(MBOX_WRITE, request);

  while (1) {
    // polls for reply by checking if its empty
    while (mmio_read(MBOX_STATUS) & MBOX_EMPTY)
      ;

    uint32_t response = mmio_read(MBOX_READ);
    if (request ==
        response) { // check if the response is the right one for our request
      // use [1] since thats how the struct is
      return mbox_msg[1] == MBOX_RESPONSE_SUCCESS;
    }
  }
  return 0;
}

// 0x00 buffer size
// 0x04 request/response code **when writing this should be 0**
// 0x08 tags for as much as u need
// 0xXX end tag (0) 4 bytes
// make sure its padded to 16 byte aligned
