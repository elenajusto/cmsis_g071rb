/* Main Program */
// Gets called after the reset handler has initialized the system.
int main(void) {
  return 0; // Do nothing so far
}

/* Startup Code */
// Reset handler
__attribute__((naked, noreturn)) void _reset(void) { // Entry point for the linker script
  // memset .bss to zero, and copy .data section to RAM region
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();             // Call main()
  for (;;) (void) 0;  // Infinite loop in the case if main() returns
}