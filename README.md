# attiny85
## Attiny85 shcematic/pcb design with example app

 * Schematic and PCB in Eagle format
 * Example app is a dual channel a/d convertor

 * **Avrdude command line**

   * sudo /usr/local/avr/bin/avrdude -c usbtiny -p t85 -V -U flash:w:analogSpi.h


 * **Avrdude fuse settings**

   * sudo /usr/local/avr/bin/avrdude -c usbtiny -p t85 -V -U lfuse:w:0xe1:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m  
