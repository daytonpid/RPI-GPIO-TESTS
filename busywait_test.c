// This code proves that it is impossible to create a 64us pulse
// with busy waits on the Rasperry Pi even with a voluntarily preemptive kernel

//	Dayton Pidhirney

//Todo DMA GPIO

#define BCM2708_PERI_BASE        0x3F000000 //RPi2|3 peripheral base offset
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

void setup_io();

int main(int argc, char **argv)
{
  // Set up gpi pointer for direct register access
  setup_io();

// Set GPIO pin 14 to output

  INP_GPIO(14); // must use INP_GPIO before we can use OUT_GPIO
  OUT_GPIO(14);

  while(1) {
    GPIO_SET = 1<<14;
    microbusy(32); //wait 32us
    GPIO_CLR = 1<<14;
    microbusy(32); //wait 32us
  }

  return 0;

}

//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;


}

//microbusy function
void microbusy(int microsec )
{
    struct timespec deadline;
    clock_gettime(CLOCK_MONOTONIC, &deadline); //Get a monotonic time
    deadline.tv_nsec += 1000;                  //Increase nsec by 1000 to match mircosec
    if(deadline.tv_nsec >= 1000000000) {
        deadline.tv_nsec -= 1000000000;
	deadline.tv_sec++;
    }

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL); //Create the sleep function
}
