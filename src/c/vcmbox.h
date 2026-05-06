#ifndef VCMBOX_H
#define VCMBOX_H
#include <stdint.h>

extern volatile uint32_t mbox_msg[36]; // gives enough space for the tags needed

enum {
  MBOX_REQUEST = 0, // for the request/response part of message when u are
                    // requesting this field must be 0
  MBOX_ENDTAG = 0   // last tag for request which is 0
};

enum { // for channels
  CHANNEL_POWER = 0,
  CHANNEL_FRAMEBUFFER = 1,
  CHANNEL_VIRTUART = 2,
  CHANNEL_VCHIQ = 3,
  CHANNEL_LED = 4,
  CHANNEL_BUTTONS = 5,
  CHANNEL_TOUCH_SCREEN = 6, // dawg where the hell is 7
  CHANNEL_ARMTOVC = 8       // property interface channel for ARM to req
};
// 0: Power management
//  1: Framebuffer
//  2: Virtual UART
//  3: VCHIQ
//  4: LEDs
//  5: Buttons
//  6: Touch screen
//  7:
//  8: Property tags (ARM -> VC)
//  9: Property tags (VC -> ARM)

enum { // tags
  // tags are both commands and buffers for responses get/set
  // general form is 0x000XYZZZ
  // X = hardware device
  // Y = command (0 = get), (4 = test), (8 = set)
  // ZZZ = specific command
  // size of value buffer where responses and params are stored u8 array
  // 0x00 tag identity
  // 0x04 value buffer size
  // 0x08 request/response code
  // 0x0c value buffer
  // padding
  //
  // for framebuffer tags cannot mix TEST with GET/SET operations and everything
  // is processed in one go
  // fb is framebuffer
  TAG_GET_FB = 0x00040001, // value buf size = 8
  TAG_FREE_FB = 0x00048001,
  TAG_SET_PHYWH = 0x00048003,      // 8
  TAG_SET_VIRTWH = 0x00048004,     // 8
  TAG_SET_VIRTOFFSET = 0x00048009, // 8
  TAG_SET_DEPTH = 0x00048005,      // 4
  TAG_SET_PXLORD = 0x0048006,      // 4
  TAG_GET_PITCH = 0x00040008,      // 4
};

uint32_t mbox_call(uint8_t channel);

#endif
