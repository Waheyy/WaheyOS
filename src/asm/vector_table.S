.balign 0x800
.global vector_table
.extern irq_el1_handler
.global install_vector_table

vector_table:
// current EL with SP0 (userland or non privileged)
// dun plan on using it so i will be lazy and not define the handlers
  .balign 0x80
  b exception_hang //synchronous

  .balign 0x80
  b exception_hang //IRQ

  .balign 0x80
  b exception_hang //FIQ

  .balign 0x80
  b exception_hang //SEerror

// current El with SPx (kernel mode)
  .balign 0x80
  b sync_el1_handler

  .balign 0x80
  b irq_el1_handler

  .balign 0x80
  b fiq_el1_handler

  .balign 0x80
  b seerror_el1_handler

// other EL entries are optional

sync_el1_handler:
  b exception_hang

fiq_el1_handler:
  b exception_hang

seerror_el1_handler:
  b exception_hang

exception_hang:
  b exception_hang

irq_el1_handler:
//save state
//stp = store pair (store pairs of registers at once)
//stores it to stack # means the value after is a constant
//! updates the sp after subtract operation
  stp x0, x1, [sp, #-16]!
  stp x2, x3, [sp, #-16]!
  stp x4, x5, [sp, #-16]!
  stp x6, x7, [sp, #-16]!
  stp x8, x9, [sp, #-16]!
  stp x10, x11, [sp, #-16]!
  stp x12, x13, [sp, #-16]!
  stp x14, x15, [sp, #-16]!
  stp x16, x17, [sp, #-16]!
  stp x18, x19, [sp, #-16]!
  stp x20, x21, [sp, #-16]!
  stp x22, x23, [sp, #-16]!
  stp x24, x25, [sp, #-16]!
  stp x26, x27, [sp, #-16]!
  stp x28, x29, [sp, #-16]! // 30 gen purpose register x30 is link reg

  str x30, [sp, #-16]!

  bl irq_el1_handler

  ldr x30, [sp], #16

  ldp x0, x1, [sp], #16
  ldp x2, x3, [sp], #16
  ldp x4, x5, [sp], #16
  ldp x6, x7, [sp], #16
  ldp x8, x9, [sp], #16
  ldp x10, x11, [sp], #16
  ldp x12, x13, [sp], #16
  ldp x14, x15, [sp], #16
  ldp x16, x17, [sp], #16
  ldp x18, x19, [sp], #16
  ldp x20, x21, [sp], #16
  ldp x22, x23, [sp], #16
  ldp x24, x25, [sp], #16
  ldp x26, x27, [sp], #16
  ldp x28, x29, [sp], #16

  eret

install_vector_table:
  adr x0, vector_table
  msr vbar_el1, x0
  ret
