# keyman64
*programmable C64 keyboard interceptor and hardware control system*

## Project status

This project is in a very early stage of development. A prototype of
the supplied pcb layout has been made and is working as intended.  The
firmware and configuration tools are functional, but bugs may still
need to be squashed.

The crosspoint IC will likely be changed to a MT8808 instead of a 
74HC22106 since the former seems to be better available and cheaper.

## Overview

The keyman64 board is intended to be installed between the C64
keyboard and the keyboard connector on the C64 mainboard. It is thus
able to intercept keystrokes from the C64 keyboard before relaying
them to the C64 CIA1.

The user can define certain key combinations to invoke internal
commands that modify the state of 16 additional general purpose
control lines provided on the board. These lines may be used to
control additional hardware installed in the C64.

The user may define any key to be used as the global command key
(hereafter noted by `<cmd>`). The arrow-left key is used by
default. When the command is simply pressed and released again, it is
relayed to the C64 as usual. But as long as it is held down, any
successive keystroke may invoke one or more user-defined commands that
modify the state of the 16 control lines.

An additional interface is provided at the C64 processor port, using
the Cassette Sense and Data lines to implement a simple serial
interface.  Bytes received on this interface are interpreted as key
codes, and the user-defined command sequences bound to the
corresponding keys are executed. This allows the control lines to be
modified by a program running on the C64 as well.

## An example application: switch the kernal and reset

Let's assume you have a dual-kernal adapter installed in your
C64. These adapters usually come with a switch that controls the
highest address line of the eprom und thus chooses the kernal that is
seen by the C64. You are expected to drill a hole in your C64 case to
install the switch.

Using keyman64 you can avoid drilling a hole in your precious case.

Simply remove the switch and connect the highest address line of the
eprom to one of the 16 control lines of the keyman64. For this
example, we'll assume that we're using the highest line of the first
control port (port a bit 7).

Create a configuration file with the following contents:

    clear port a
    clear port b

    k: invert port a bit 7

Convert it using the `keyman64` command line tool:

    keyman64 example.conf example.bin

And write the resulting binary file to the avr's eprom:

    avrdude -p m1284p -c usbasp -U eeprom:w:example.bin:r

Then reset the keyman64. It will first execute any command not bound
to a key. This allows us to set the initial state of the control
lines. These 16 lines are organized in two 8-bit wide ports, denoted
`port a` and `port b` in the configuration file. In this case, all
lines are initially pulled low using the `clear` command).

Now the key combination `<cmd>-k` will invert the state of the
eprom address line, effectively switching back and forth between the
two kernal images on each invocation.

While this does the job on the hardware level, it might still cause
the C64 to crash, since you might swap the kernal contents while
kernal code is being executed. So it would be nice to also reset the
C64 after switching the kernal rom.

We'll connect the C64 reset line to the first line of the first
control port and change the configuration to:

    tristate port a bit 0
    clear port a bits 1-7
    clear port b

    r: clear port a bit 0
    r: sleep 10
    r: tristate port a bit 0

    k: invert port a bit 7
    k: exec r

Now if we press `<cmd>-r`, the reset line will be driven from tristate
to low for 10 milliseconds and is then tristated again, effectively
causing the C64 to reset.

And if we press `<cmd>-k`, the kernal will be switched just like
before, but then the reset sequence bound to `<cmd>-r` will be
executed in addition.

Thus we can switch the kernal and immediately reset the C64 simply by
pressing `<cmd>-k` instead of having to turn off the C64, flip a
switch and turn the C64 back on every time we want to change the
kernal.

## Defining keyboard macros

The `type` command can be bound to a key as well:

    d: type load"$",8~
    
Now pressing `<cmd>-d` will type `load"$",8<return>`. The `~` 
symbol denotes a newline in this context.

## Remapping the keyboard

The `swap` command can be used to virtually swap two keys on the
keyboard. For example, to change the keyboard layout to QWERTZ, just add

    swap z y

to your configuration.

## Entering the bootloader

The Atmega needs to enter its bootloader in order to write the configuration
or to update the firmware. Entering the bootloader can be achived by pressing the
RESET button while holding down the HWBE button. Alternatively, the `boot` command
can be used to enter the bootloader using a key combination, e.g

    B: boot
    
will make `<cmd>-b` enter the bootloader. The bootloader will exit automatically 
after the configuration or firmware has been writen.

## Hardware design

An Atmel 1284P microprocessor is used to implement the core
functionality. It is able to scan the C64 keyboard matrix using only
two io lines with the help of an external circuit constructed from a
4520 (dual four bit binary counter) and two 4051 (analog 1-to-8
multiplexers). This keeps 16 of the 32 io lines of the microprocessor
free for general purpose control of external hardware. The remainder
of the microprocessors io lines are used to control a 7422106 8x8
crosspoint switch IC. It provides an 8x8 matrix of analog switches,
which is connected to the CIA1 of the C64, and just looks like a real
keyboard matrix to the CIA.

The user configuration can be created with the supplied command line
tool and is stored in the microprocessors on-board eeprom. To make it
easy for the user to transfer the configuration, a USB header is
connected to the microprocessor, which must be fitted with a usb
bootloader beforehand (e.g. usbasp).

## License

<pre>
             keyman64 Hardware, Firmware, and Software
     Copyright (c) 2015, Henning Bekel &lt;h.bekel@googlemail.com&gt;
	   
                      All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</pre>


