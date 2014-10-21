MEMORY
{
    RAM : ORIGIN = 0x8000, LENGTH = 256M
}

heap_size = 200M;

SECTIONS
{
	.init : 
	{
		*(.init)
		*(.led)
		*(.mem)
	} > RAM

	. = ALIGN(4096);

	.data :
	{
		*(.data*)
	} > RAM

	COMMON :
	{
		*(COMMON)
	} > RAM

    .text :
	{
	  *(.text)          /* .text sections (code) */
	  *(.text*)             /* .text* sections (code) */
	  *(.rodata)            /* .rodata sections (constants, strings, etc.) */
	  *(.rodata*)           /* .rodata* sections (constants, strings, etc.) */
	} > RAM
   
    . = ALIGN(4096); /* align to page size */
      
    .bss :
	{
   		_bss_start = .;
		*(.bss*)
	} > RAM
   
    . = ALIGN(4096); /* align to page size */

	_bss_end = .;

	. = ALIGN(4);

	.heap :
	{
	    _heap_start = .;
	    . = . + heap_size;
	            _heap_end = .;
	} > RAM
   
}
