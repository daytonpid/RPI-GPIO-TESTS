/*
   led_test.c
   Public Domain
   Dayton Pidhirney <deepsixsec@protonmail.com>
 
An uneccesarily complex program to control Raspberry Pi GPIO's for LED's
or anything that takes always on high/low logic levels.
*/

/* Compiling:
   gcc -o led_test led_test.c
*/

/* Usage:
   sudo ./led_test [GPIO] ... [OPTION] ...
      -h    sets LED to high
      -l    sets LED to low
   
   EXAMPLES:
      sudo ./led_test 14 -h

         sets LED on GPIO 14 to high

      sudo ./led_test 14 15 16 -l

         sets LED on GPIO 14 15 and 16 to low
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

/* GPIO register length */
#define GPIO_LEN  0xB4
#define SYST_LEN  0x1C
#define BSCS_LEN  0x40

#define GPSET0 7
#define GPSET1 8
#define GPCLR0 10
#define GPCLR1 11
#define GPLEV0 13
#define GPLEV1 14

#define PI_BANK (gpio>>5)
#define PI_BIT  (1<<(gpio&0x1F))

/* GPIO modes */
#define PI_INPUT  0
#define PI_OUTPUT 1

#define MAX_GPIOS 32

static volatile uint32_t piPeriphBase = 0x3F000000;
static volatile uint32_t piBusAddr = 0x40000000;
static volatile uint32_t  *gpioReg = MAP_FAILED;
static volatile uint32_t  *systReg = MAP_FAILED;
static volatile uint32_t  *bscsReg = MAP_FAILED;

#define SYST_BASE  (piPeriphBase + 0x003000)
#define GPIO_BASE  (piPeriphBase + 0x200000)
#define BSCS_BASE  (piPeriphBase + 0x214000)

static uint32_t g_mask;

static int g_num_gpios;
static int g_gpio[MAX_GPIOS];
static int state;

/* Program usage details */
void usage()
{
   fprintf
   (stderr,
      "\n" \
      "Usage: sudo ./led_test [GPIO] ... [OPTION] ...\n" \
      "\nOPTIONS:\n\n" \
      "   -h    sets LED to high \n" \
      "   -l    sets LED to low \n" \
      "\n\nEXAMPLES:\n\n" \
      "   sudo ./led_test 14 -h\n\n" \
      "       sets LED on GPIO 14 to high\n\n" \
      "   sudo ./led_test 14 15 16 -l\n\n" \
      "       sets LED on GPIO 14 15 and 16 to low" \
      "\n"
   );
}

/* Fatal function */
void fatal(int show_usage, char *fmt, ...)
{
   char buf[128];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   fprintf(stderr, "%s\n", buf);

   if (show_usage) usage();

   fflush(stderr);

   exit(-1);
}

void gpioSetMode(unsigned gpio, unsigned mode)
{
   int reg, shift;

   reg   =  gpio/10;
   shift = (gpio%10) * 3;

   gpioReg[reg] = (gpioReg[reg] & ~(7<<shift)) | (mode<<shift);
}


int gpioRead(unsigned gpio)
{
   if ((*(gpioReg + GPLEV0 + PI_BANK) & PI_BIT) != 0) return 1;
   else                                         return 0;
}

void gpioWrite(unsigned gpio, unsigned level)
{
   if (level == 0) *(gpioReg + GPCLR0 + PI_BANK) = PI_BIT;
   else            *(gpioReg + GPSET0 + PI_BANK) = PI_BIT;
}


/* Map register virtual address */
static uint32_t * initMapMem(int fd, uint32_t addr, uint32_t len)
{
    return (uint32_t *) mmap(0, len,
       PROT_READ|PROT_WRITE|PROT_EXEC,
       MAP_SHARED|MAP_LOCKED,
       fd, addr);
}

/* Set up a memory regions to access GPIO */
int gpioInitialise(void)
{
   int fd;

   /* Open /dev/mem */
   fd = open("/dev/mem", O_RDWR | O_SYNC) ;

   if (fd < 0)
   {
      fprintf(stderr,
         "This program needs root privileges.  Try using sudo\n");
      return -1;
   }

   /* mmap GPIO */
   gpioReg  = initMapMem(fd, GPIO_BASE,  GPIO_LEN);
   systReg  = initMapMem(fd, SYST_BASE,  SYST_LEN);
   bscsReg  = initMapMem(fd, BSCS_BASE,  BSCS_LEN);

   close(fd);

   /* If mmap failed maping to virtual fail */
   if ((gpioReg == MAP_FAILED) ||
       (systReg == MAP_FAILED) ||
       (bscsReg == MAP_FAILED))
   {
      fprintf(stderr,
         "Bad, mmap failed\n");
      return -1;
   }
   return 0;
}

/* Determine eventual GPIO state (on/off)*/
static int initOpts(int argc, char *argv[])
{
   int i, opt;

   while ((opt = getopt(argc, argv, ":hl")) != -1)
   {
      i = -1;

      switch (opt)
      {
         case 'h':
            state = 1;
            break;

         case 'l':
            state = 0;
            break;

        default: /* '?' */
           usage();
           exit(-1);
        }
    }
   return optind;
}

/* What you are here for */
int main(int argc, char *argv[])
{
   int i, rest, g, mode;

   /* Command line parameters */
   rest = initOpts(argc, argv);

   g_num_gpios = 0;

   /* Read user input for GPIO number */
   for (i=rest; i<argc; i++)
   {
      g = atoi(argv[i]);
      if ((g>=0) && (g<32))
      {
         g_gpio[g_num_gpios++] = g;
         g_mask |= (1<<g);
      }
      else fatal(1, "ERROR:\n\n    GPIO (%d)? .... Hey! That's not a valid GPIO number, ya hear?!\n", g);
   }

   /* Unspecified GPIO */
   if (!g_num_gpios) fatal(1, "ERROR:\n\n    I can't turn on your LED's if you don't tell me which one to turn on .... d'ohhh!");

   /* Initialise GPIO */
   if (gpioInitialise() < 0)
   {
      fprintf(stderr, "ERROR:\n\n    pigpio failed to initialize .... good luck figuring out why! \n");
      return 1;
   }

   /* Set GPIO direction */
   mode = PI_OUTPUT;

   /* Set GPIO modes */
   for (i=0; i<g_num_gpios; i++) gpioSetMode(g_gpio[i], mode);

   /* Write pins */
   for (i=0; i<g_num_gpios; i++) gpioWrite(g_gpio[i], state);

   /* See ya later ;) */
   return 0;
}
