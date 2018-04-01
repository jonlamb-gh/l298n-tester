# l298n-tester

## TODOs

- list hw, etc
- create diagram
- inlining, optimizations

## Hardware

- [teensy++ 2.0](https://www.pjrc.com/store/teensypp.html)
    - [AT90USB1286](http://ww1.microchip.com/downloads/en/DeviceDoc/doc7593.pdf)
    - 8 bit AVR
    - 16 MHz
- [l298 H-Bridge board](https://www.sparkfun.com/products/9670)
- buttons, switches, potentiometers, etc

## PWM

[Teensy++ 2.0 pwm frequency](https://www.pjrc.com/teensy/td_pulse.html)

| timer | pwm pins   | default frequency |
| ----- | --------   | ----------------- |
| 0     | D0         | 976.56 Hz  |
| 1     | B5, B6, B7 | 3921.57 Hz |
| 2     | D1, B4     | 3921.57 Hz |
| 3     | C4, C5, C6 | 3921.57 Hz |
