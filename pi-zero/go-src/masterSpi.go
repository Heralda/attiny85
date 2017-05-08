package main

import (
	"fmt"
	"os"
	"os/signal"
	"time"

	"github.com/davecheney/gpio"
	"github.com/davecheney/gpio/rpi"
)

var (
	dataIn , dataOut , dataClk ,dataReset gpio.Pin
	delay time.Duration = 1
)

func initSpi() {
	// set the spi pins and mode
	di, err := gpio.OpenPin(rpi.GPIO23,gpio.ModeInput)
	do, err := gpio.OpenPin(rpi.GPIO27, gpio.ModeOutput)
	clk, err := gpio.OpenPin(rpi.GPIO22, gpio.ModeOutput)
	reset, err := gpio.OpenPin(rpi.GPIO24, gpio.ModeOutput)

	if err != nil {
		fmt.Printf("Error opening pin! %s\n", err)
		return
	}

	dataIn = di
	dataOut = do
	dataClk = clk
	dataReset = reset

	dataOut.Clear()
	dataClk.Clear()
	dataReset.Clear()

	fmt.Printf("GPIO data pins initialised ")
}

func writeSpi(data uint8) {
	dataOut.Clear()
	var mask uint8 = 0x80

	for mask != 0 {
		if (data & mask == mask) {
			dataOut.Set()
		} else {
			dataOut.Clear()
		}
		time.Sleep(delay * time.Millisecond)
		dataClk.Set()
		time.Sleep(delay * time.Millisecond)
		dataClk.Clear()
		mask = mask >> 1
	}
}


func readSpi() uint8 {
	var ret, mask uint8 = 0x00,0x80
	for mask != 0 {
		dataClk.Set()
		time.Sleep(delay * time.Millisecond)
		if (dataIn.Get()) {
			ret |= mask
		}
		dataClk.Clear()
		time.Sleep(delay * time.Millisecond)
		mask = mask >> 1
	}
	return ret
}

func main() {

	var readLoA,readHiA,readLoB,readHiB,addr uint8 = 0xFF,0xFF,0xFF,0xFF,0x03
	var result uint16
	var voltage float32

	// turn the led off on exit
	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	go func() {
		for _ = range c {
			fmt.Printf("\nClearing and unexporting the pin.\n")
			dataOut.Clear()
			dataClk.Clear()
			dataOut.Close()
			dataIn.Close()
			dataClk.Close()
			dataReset.Close()
			os.Exit(0)
		}
	}()

	initSpi()
	dataReset.Set()
	time.Sleep(200 * time.Millisecond)

	for {
		writeSpi(addr)
		time.Sleep(50 * time.Millisecond)
		readLoA = readSpi()
		time.Sleep(50 * time.Millisecond)
		readHiA = readSpi()
		time.Sleep(50 * time.Millisecond)
		readLoB = readSpi()
		time.Sleep(50 * time.Millisecond)
		readHiB = readSpi()

		if (readLoA != addr && (readHiA <= 0x03) ) {
			fmt.Printf("Result A hi,lo  byte from remote sensor %x %x\n", readHiA,readLoA )
			result = uint16( (256*uint16(readHiA)) + uint16(readLoA))
			fmt.Printf("Value A in decimal %d\n",result)
			voltage = float32(2.56)*float32(result)/float32(1024)
			fmt.Printf("Voltage A measured %f\n\n",voltage)
			fmt.Printf("Result B hi,lo  byte from remote sensor %x %x\n", readHiB,readLoB )
			result = uint16( (256*uint16(readHiB)) + uint16(readLoB))
			fmt.Printf("Value B in decimal %d\n",result)
			voltage = float32(2.56)*float32(result)/float32(1024)
			fmt.Printf("Voltage B measured %f\n",voltage)
		} else {
			fmt.Printf("Nothing to evaluate invalid address\n")
		}
	}
}
