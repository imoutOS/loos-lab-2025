
ENTRY(_start)
SECTIONS
  {
  . = 0x9000000090000000;
  _kernel_start = .;

  .text : {
      *(._start)
      *(.text)
    . = ALIGN(0x4000);
    _tlbrentry = .;
    *(tlbrentry)
    . = ALIGN(0x4000);
    *(trapentry)
    . = ALIGN(0x10);
  }
  . = ALIGN(16);

  .data : {
      *(.rodata)
      *(.data)
  }
  . = ALIGN(16);

  _bss_start = .;
  .bss : {
      *(.bss)
  }
  _bss_end = .;

  _stack_top = ALIGN(16);
  . = _stack_top + 0x4000;
  _stack_pointer = .;

  end = .;
  _end = .;

  . = ALIGN(0x1000);
  _heap_start = .;
  . = _heap_start + 0x20000;
  _heap_end = .;

  _kernel_end = .;
  kernel_end = .;
}
