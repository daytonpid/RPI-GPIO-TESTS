RPI-GPIO-TESTS

Run `make mhztest` for checking maximum supported gpio frequency on GPIO 14

Run `make freq_count_1` for testing gpio pulse delay

Ensure that Pigpio is installed on your Pi before attempting to compile freq_count_1
http://abyz.co.uk/rpi/pigpio/download.html

freq_count_1 example: 

`sudo ./freq_count_1 14 -p32` This will create a 64us pulse on gpio 14

`sudo ./freq_count_1 4 14 15 -p32` this will create a 64us pulse on gpio 4, 14, and 15 
