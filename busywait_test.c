// This code proves that it is possible to create a 64us pulse
// with busy waits on the Rasperry Pi

// Dayton Pidhirney

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pigpio.h>

#define MAX_GPIOS 32

//-p flag constraints
#define OPT_P_MIN 1
#define OPT_P_MAX 1000
#define OPT_P_DEF 32

static volatile int g_pulse_count[MAX_GPIOS];
static volatile int g_reset_counts;
static uint32_t g_mask;

static int g_num_gpios;
static int g_gpio[MAX_GPIOS];

static int g_opt_p = OPT_P_DEF;
static int g_opt_t = 0;

void usage()
{
   fprintf
   (stderr,
      "\n" \
      "Usage: sudo ./busywait_test.c [GPIO VALUE] ... [OPTION] ...\n" \
      "   -p value, sets pulses every given value in microseconds, %d-%d, TESTING only\n" \
      "\nEXAMPLE\n" \
      "sudo ./freq_count_1 14 4 7 -p32\n" \
      "Monitor gpios 14, 4 and 7. Set pin 32us, then clear pin 32us later. This creates a 64\n" \
      "\n",
      OPT_P_MIN, OPT_P_MAX, OPT_P_DEF
   );
}

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

   exit(EXIT_FAILURE);
}

static int initOpts(int argc, char *argv[])
{
   int i, opt;

   while ((opt = getopt(argc, argv, ":p")) != -1)
   {
      i = -1;

      switch (opt)
      {
         case 'p':
            i = atoi(optarg);
            if ((i >= OPT_P_MIN) && (i <= OPT_P_MAX))
               g_opt_p = i;
            else fatal(1, "invalid -p option (%d)", i);
            g_opt_t = 1;
            break;

        default: /* '?' */
           usage();
           exit(-1);
        }
    }
   return optind;
}

void edges(int gpio, int level, uint32_t tick)
{
   int g;

   if (g_reset_counts)
   {
      g_reset_counts = 0;
      for (g=0; g<MAX_GPIOS; g++) g_pulse_count[g] = 0;
   }

   /* only record low to high edges */
   if (level == 1) g_pulse_count[gpio]++;
}

int main(int argc, char *argv[])
{
   int i, rest, g, wave_id, mode;
   gpioPulse_t pulse[2];
   int count[MAX_GPIOS];

   /* command line parameters */

   rest = initOpts(argc, argv);

   /* get the gpios to monitor */

   g_num_gpios = 0;

   for (i=rest; i<argc; i++)
   {
      g = atoi(argv[i]);
      if ((g>=0) && (g<32))
      {
         g_gpio[g_num_gpios++] = g;
         g_mask |= (1<<g);
      }
      else fatal(1, "%d is not a valid g_gpio number\n", g);
   }

   if (!g_num_gpios) fatal(1, "At least one gpio must be specified");

   printf("Monitoring gpios");
   for (i=0; i<g_num_gpios; i++) printf(" %d", g_gpio[i]);
   printf("\nPulse width in microseconds (%d)", g_opt_p);

   if (gpioInitialise()<0) return 1;

   gpioWaveClear();

   pulse[0].gpioOn  = g_mask;
   pulse[0].gpioOff = 0;
   pulse[0].usDelay = g_opt_p;

   pulse[1].gpioOn  = 0;
   pulse[1].gpioOff = g_mask;
   pulse[1].usDelay = g_opt_p;

   gpioWaveAddGeneric(2, pulse);

   wave_id = gpioWaveCreate();

   /* monitor g_gpio level changes */

   for (i=0; i<g_num_gpios; i++) gpioSetAlertFunc(g_gpio[i], edges);

   mode = PI_INPUT;

   if (g_opt_t)
   {
      gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT);
      mode = PI_OUTPUT;
   }

   for (i=0; i<g_num_gpios; i++) gpioSetMode(g_gpio[i], mode);

   while (1)
   {
      for (i=0; i<g_num_gpios; i++) count[i] = g_pulse_count[g_gpio[i]];

      g_reset_counts = 1;

      for (i=0; i<g_num_gpios; i++)
      {
         printf(" %d=%d", g_gpio[i], count[i]);
      }

      printf("\n");

      gpioDelay(g_opt_p * 100000);
   }

   gpioTerminate();
}