#include <bcm2835.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h> 

int interval = -1;
int a_pin = -1, b_pin = -1;
long lower = -1, upper = -1;
int cal = 0;
char * handler = NULL;
int last_state = -1;
int nstates = 0;

void set_output(int pin, int outp)
{
  bcm2835_gpio_fsel(pin, outp ? BCM2835_GPIO_FSEL_OUTP : BCM2835_GPIO_FSEL_INPT);
}

void set_high(int pin, int high)
{
  bcm2835_gpio_write(pin, high ? HIGH : LOW);
}

int is_high(int pin)
{
  return bcm2835_gpio_lev(pin) == HIGH;
}

void discharge(int pin_a, int pin_b)
{
  set_output(pin_a, 0);
  set_output(pin_b, 1);
  set_high(pin_b, 0);
}

long charge_time(int pin_a, int pin_b)
{
  set_output(pin_a, 1);
  set_output(pin_b, 0);
  struct timespec start = {0};
  clock_gettime(CLOCK_REALTIME, &start);
  set_high(pin_a, 1);
  struct timespec finish = {0};
  int counter = 0;
  while (!is_high(pin_b))
  {
    ++counter;
    if (counter > 100000)
    {
      counter = 0;
      clock_gettime(CLOCK_REALTIME, &finish);
      if (finish.tv_sec - start.tv_sec >= 1) return -1;
    }
  }
  clock_gettime(CLOCK_REALTIME, &finish);
  return (finish.tv_sec - start.tv_sec)*1e9+finish.tv_nsec - start.tv_nsec;
}
 
long measure(int pin_a, int pin_b)
{
  long res = charge_time(pin_a, pin_b);
  discharge(pin_a, pin_b);
  return res;
}

void report_error(const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(1);
}

int convert_pin(int pin) {
  switch(pin) {
    //case 1: return RPI_BPLUS_GPIO_J8_01;
    //case 2: return RPI_BPLUS_GPIO_J8_02;
  case 3: return RPI_BPLUS_GPIO_J8_03;
    //case 4: return RPI_BPLUS_GPIO_J8_04;
  case 5: return RPI_BPLUS_GPIO_J8_05;
    //case 6: return RPI_BPLUS_GPIO_J8_06;
  case 7: return RPI_BPLUS_GPIO_J8_07;
  case 8: return RPI_BPLUS_GPIO_J8_08;
    //case 9: return RPI_BPLUS_GPIO_J8_09;
  case 10: return RPI_BPLUS_GPIO_J8_10;
  case 11: return RPI_BPLUS_GPIO_J8_11;
  case 12: return RPI_BPLUS_GPIO_J8_12;
  case 13: return RPI_BPLUS_GPIO_J8_13;
    //case 14: return RPI_BPLUS_GPIO_J8_14;
  case 15: return RPI_BPLUS_GPIO_J8_15;
  case 16: return RPI_BPLUS_GPIO_J8_16;
    //case 17: return RPI_BPLUS_GPIO_J8_17;
  case 18: return RPI_BPLUS_GPIO_J8_18;
  case 19: return RPI_BPLUS_GPIO_J8_19;
    //case 20: return RPI_BPLUS_GPIO_J8_20;
  case 21: return RPI_BPLUS_GPIO_J8_21;
  case 22: return RPI_BPLUS_GPIO_J8_22;
  case 23: return RPI_BPLUS_GPIO_J8_23;
  case 24: return RPI_BPLUS_GPIO_J8_24;
    //case 25: return RPI_BPLUS_GPIO_J8_25;
  case 26: return RPI_BPLUS_GPIO_J8_26;
    //case 27: return RPI_BPLUS_GPIO_J8_27;
    //case 28: return RPI_BPLUS_GPIO_J8_28;
  case 29: return RPI_BPLUS_GPIO_J8_29;
    //case 30: return RPI_BPLUS_GPIO_J8_30;
  case 31: return RPI_BPLUS_GPIO_J8_31;
  case 32: return RPI_BPLUS_GPIO_J8_32;
  case 33: return RPI_BPLUS_GPIO_J8_33;
    //case 34: return RPI_BPLUS_GPIO_J8_34;
  case 35: return RPI_BPLUS_GPIO_J8_35;
  case 36: return RPI_BPLUS_GPIO_J8_36;
  case 37: return RPI_BPLUS_GPIO_J8_37;
  case 38: return RPI_BPLUS_GPIO_J8_38;
    //case 39: return RPI_BPLUS_GPIO_J8_39;
  case 40: return RPI_BPLUS_GPIO_J8_40;
  default: report_error("Unsupported pin number %i\n", pin);
  }
  return -1;
}

int calc_state(long val)
{
  long step = (upper - lower)/nstates;
  //printf("l: %li, u: %li, v: %li, s: %li\n", lower, upper, val, step);
  if (val < lower + step/2) return 0;
  if (val > upper - step/2) return nstates-1;
  return 1 + (val - lower - step/2)/step;
}

static void cleanup()
{
  discharge(a_pin, b_pin);
}

static void calibration_finished()
{
  report_error("Calibration finished lower: %i upper: %i\n", lower, upper);
}

void calibrate()
{
  signal(SIGINT, &calibration_finished);
  lower = upper = -1;
  while (1)
  {
    int val = measure(a_pin, b_pin);
    if (lower == -1 || lower > val) lower = val;
    if (upper == -1 || upper < val) upper = val;
    printf("%i\n", val);
    delay(interval);
  }
}

void handle_state(int cur, int no_fork)
{
  if (last_state == cur) return;
  static int MAX = 10;
  char prev_str[MAX];
  char cur_str[MAX];
  snprintf(prev_str, MAX, "%i", last_state);
  snprintf(cur_str, MAX, "%i", cur);
  if (no_fork || !fork())
  {
    execl("/bin/sh", "/bin/sh", handler, prev_str, cur_str, NULL);
    exit(0);//just to be sure
  }
  last_state = cur;
}

void watch_finished()
{
  handle_state(-1, 1);
  exit(0);
}

void watch()
{
  signal(SIGTERM, &watch_finished);
//  on_exit(&watch_finished, NULL);
  while (1)
  {
    long val = measure(a_pin, b_pin);
    if (val == -1) continue;
    int state = calc_state(val);
    //    printf("state: %i, val: %li\n", state, val);
    handle_state(state, 0);
    delay(interval);
  }
}

int main(int argc, char **argv)
{
  int c;
  while ((c = getopt(argc, argv, "a:b:l:u:i:ch:n:")) != -1)
  {
    long val;
    if (c != '?' && c != 'c' && c != 'h')
    {
      char * end = NULL;
      val = strtol(optarg, &end, 10);
      if (*end != 0) report_error("Parameter %c should be integer, not %s\n", c, optarg);
    }
    switch (c)
    {
    case 'c':
      cal = 1;
      break;
    case 'a':
      a_pin = val;
      break;
    case 'b':
      b_pin = val;
      break;
    case 'l':
      lower = val;
      break;
    case 'u':
      upper = val;
      break;
    case 'i':
      interval = val;
      break;
    case 'n':
      nstates = val;
      break;
    case 'h':
      handler = optarg;
      break;
    default:
      report_error("Unknown argument %c\n", c == '?' ? optopt : c);
    }
  }
  if (interval < 0) report_error("Interval is not specified\n");
  if (a_pin < 0) report_error("A pin is not specified\n");
  if (b_pin < 0) report_error("B pin is not specified\n");
  if (!cal && lower < 0) report_error("Lower bound is not specified\n");
  if (!cal && upper < 0) report_error("Upper bound is not specified\n");
  if (!cal && !handler) report_error("Handler is not specified\n");
  if (!cal && nstates <= 0) report_error("Number of states is not specified\n");
  a_pin = convert_pin(a_pin);
  b_pin = convert_pin(b_pin);
  
  if (!bcm2835_init()) report_error("Initialization failed\n");

  on_exit(&cleanup, NULL);
  if (cal)
    calibrate(a_pin, b_pin, interval);
  else
    watch();
  report_error("Finished\n");
  return 0;
}
