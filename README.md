#RPI-GPIO-TESTS

`make busywait_test` to check minimum supported busy wait delay on RPi2 and RPi3 GPIO('s)

`make led_test` to set a GPIO('s) connected LED to either high or low using a easy interface

Ensure that Pigpio is installed on your Pi before attempting to compile `busywait_test`: http://abyz.co.uk/rpi/pigpio/download.html

###busywait_test usage/example: 

```
Pulse GPIO 4 (default settings)
sudo ./freq_count_1  4

Pulse GPIO 4 and 8 (default settings)
sudo ./freq_count_1  4 8

Pulse 2us edges on GPIO 4, 7, 8, and 9 (4us square wave, 250000 highs per second).
sudo ./freq_count_1  4 7 8 9 -p 2

Pulse 32us edges on GPIO 4, 7, 8, and 9  (64us square wave, 3200000 highs per second).
sudo ./freq_count_1  4 7 8 9 -p 2
*/
```

###led_test usage/example:

```
Usage:
   sudo ./led_test [GPIO] ... [OPTION] ...
      -h    sets LED to high
      -l    sets LED to low
   
   EXAMPLES:
      sudo ./led_test 14 -h
         sets LED on GPIO 14 to high
      sudo ./led_test 14 15 16 -l
         sets LED on GPIO 14 15 and 16 to low
```
