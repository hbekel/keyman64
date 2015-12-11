# keyman64
*programmable C64 keyboard interceptor and hardware control system*

The keyman64 is a programmable keyboard interceptor and hardware
control system for computers equipped with a simple 64-key matrix
keyboard.

It is installed between the keyboard and the computer, continually
scanning the keyboard matrix and relaying the keyboard state to the
computer using a crosspoint switch. To the computer, the crosspoint
switch matrix looks just like a physical keyboard, while the keyman64
gains the ability to intercept keystrokes and control the matrix seen
by the computer.

The keyman64 can be configured to intercept special key combinations
and invoke arbitrary sequences of commands to alter the state of
sixteen general purpose control lines provided on the board. These
lines can be used to control additional hardware instead of using
physical buttons or switches.

Additional features include the ability to send predefined keyboard
macros or to redefine the keyboard layout. Commands can also be sent
from a remote PC via USB, or send on simple serial wire interface,
allowing remote control from either a PC or another microcontroller or
similar device.

*For some concrete examples of what the keyman64 can do, see the
[configuration
examples](http://www.henning-bekel.de/keyman64/#configuration-examples).*

Please refer to the [project
page](http://www.henning-bekel.de/keyman64) for detailed
documentation.
