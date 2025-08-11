ENTRY(_reset); /* entry point for the linker script */

MEMORY {
  flash(rx)  : ORIGIN = 0x08000000, LENGTH = 128k  /* flash for STM32G071XX */
  sram(rwx) : ORIGIN = 0x20000000, LENGTH = 36k  /* SRAM for STM32G071XX */
}
_estack     = ORIGIN(sram) + LENGTH(sram);    /* stack points to end of SRAM */

SECTIONS {

  /* Place vector table, firmware code and read only data in memory */
  .vectors  : { KEEP(*(.vectors)) }   > flash
  .text     : { *(.text*) }           > flash
  .rodata   : { *(.rodata*) }         > flash

  /* Copy data to SRAM at startup */
  .data : {
    _sdata = .;   /* .data section start */
    *(.first_data)
    *(.data SORT(.data.*))
    _edata = .;  /* .data section end */
  } > sram AT > flash
  _sidata = LOADADDR(.data);

  /* Copy zero initialized data to SRAM */
  .bss : {
    _sbss = .;              /* .bss section start */
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;              /* .bss section end */
  } > sram

  . = ALIGN(8);
  _end = .;     /* for cmsis_gcc.h  */
}
