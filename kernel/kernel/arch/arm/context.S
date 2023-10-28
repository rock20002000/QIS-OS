  .global get_arm_private_peripheral_base
  @ uint32_t get_arm_private_peripheral_base(void)@
  .func get_arm_private_peripheral_base
get_arm_private_peripheral_base:

  @ Get base address of private perpherial space
  mrc     p15, 4, r0, c15, c0, 0  @ Read periph base address
  bx      lr

  .endfunc    @get_arm_private_peripheral_base()@

  