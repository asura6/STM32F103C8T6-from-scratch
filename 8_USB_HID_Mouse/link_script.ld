/* Linkfile for a STM32F103*
 * Robin Isaksson 2016 
 */ 

/* MEMORY STRUCTURE
 *  Program flash:   64 KB: 0x0000_0000 -> 0x0000_FFFF 
 *  RAM:             20 KB: 
 */

MEMORY { 
    FLASH :      ORIGIN = 0x08000000, LENGTH = 64K 
    RAM :       ORIGIN = 0x20000000, LENGTH = 20K 
} 

    _STACK_TOP = ORIGIN(RAM) + LENGTH(RAM);
    . = ORIGIN(FLASH); 



SECTIONS {


   .text : {
       . = ALIGN(4);
       KEEP ( * (vectors) );
       . = ALIGN(4);
       * (.text); 
       * (.rodata); 
       /* _etext */
       . = ALIGN(4);
       /*_exit = .;*/
    } > FLASH

       . = ALIGN(4);
    FLASH_DATA_START = .; /*Where we will load from flash when writing to RAM */ 

    /* Now we'll configure where we will keep our data outside off flash */ 
    . = ORIGIN(RAM);
    RAM_DATA_START = .; 

    .data : AT (FLASH_DATA_START) {
       . = ALIGN(4);
        * (.data);
    } > RAM
       . = ALIGN(4);
    RAM_DATA_END = .;
    RAM_DATA_SIZE = RAM_DATA_END - RAM_DATA_START;

    __bss_start__ = .;
    .bss : {
       . = ALIGN(4);
        * (.bss); 
    } > RAM
       . = ALIGN(4);
    __bss_end__ = .;
    bss_size = __bss_end__ - __bss_start__; 
    end =.;

/*    .heap : {             
        end = .;
        _heap_start = .; 
        . += heap_size;
        _heap_end = .;
    } > RAM  */
} 
