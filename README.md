#RPI-GPIO-TESTS

Run `make busywait_test` to check maximum supported busy wait speed on RPi2|3 on `GPIO 14` (you can change this in code)

Run `make pigpio_pulse` to test pigpio GPIO square wave pulses on RPi

Ensure that Pigpio is installed on your Pi before attempting to compile `pigpio_pulse`: http://abyz.co.uk/rpi/pigpio/download.html

pigpio_pulse example: 

`sudo ./pigpio_pulse 14 -p32` This will create a 64us square wave on GPIO 14

`sudo ./pigpio_pulse 4 14 15 -p32` this will create a 64us square wave on GPIO 4, 14, and 15 
