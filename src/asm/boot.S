.section ".text.boot"

.global _start

_start:
  mrs x1, mpidr_el1
  and x1, x1, #3
  cbz x1, maincore
1: wfe
  b 1b

maincore:
  ldr x1, =_start
  mov sp, x1

  ldr x1, =bss_begin
  ldr x2, =bss_end

clearbss:
  str xzr, [x1], #8
  cmp x1, x2
  bne clearbss

  b kernel_entry

