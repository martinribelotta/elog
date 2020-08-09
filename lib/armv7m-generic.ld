/* Copyright (C) 2020 Martin Ribelotta.

   This file is part of tdebug.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

ENTRY(Reset_Handler)

/* Specify the memory areas */
MEMORY
{
   RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 8K
   ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
   LOG (r)   : ORIGIN = 0xF0000000, LENGTH = 1024M
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into ROM */
  .isr_vectors :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vectors))
    . = ALIGN(4);
  } >ROM

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    . = ALIGN(4);
    _etext = .;
  } >ROM

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >ROM

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ROM

  . = ALIGN(4);

  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >ROM

  . = ALIGN(4);

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ROM

  . = ALIGN(4);

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ROM

  . = ALIGN(4);

/*
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ROM
*/
  _sidata = LOADADDR(.data);

  .data : 
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> ROM

  . = ALIGN(4);
  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
    __end__ = .;
    end = .;
  } >RAM


  .elog :
  {
    *(elog* elog.*)
  } >LOG

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
    *(.fini_array*)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
