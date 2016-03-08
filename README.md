# attiny85
## Attiny85 shcematic/pcb design with example app

 * Schematic and PCB in Eagle format
 * ### Example app is a dual channel a/d convertor

 * **Avrdude command line**

   * sudo /usr/local/avr/bin/avrdude -c usbtiny -p t85 -V -U flash:w:analogSpi.hex


 * **Avrdude fuse settings**

   * sudo /usr/local/avr/bin/avrdude -c usbtiny -p t85 -V -U lfuse:w:0xe1:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 


 * ### The vusbtiny application is a usb bootloader and used to program the attiny device
   
   * The purpose of this application was to use the design I made and create a usbtiny programmer for all my attiny projects
  
   * Updated the usbdrv.c and usbconfig.h file to configure the port assigments to match the pcb design

 * **Avrdude command line**

   * sudo /usr/local/avr/bin/avrdude -c usbtiny -p t85 -V -U flash:w:vusbtiny.hex


  * **Avrdude fuse settings**

   * sudo /usr/local/avr/bin/avrdude -c usbtiny -p t45 -V -U lfuse:w:0xe1:m -U hfuse:w:0x5d:m -U efuse:w:0xff:m
