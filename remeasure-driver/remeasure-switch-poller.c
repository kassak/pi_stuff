#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "remeasure-client.h"

int interval = -1;
uint32_t lower = -1, upper = -1;
int cal = 0;
char * handler = NULL;
int last_state = -1;
int nstates = 0;
int use_exec = 0;

void report_error(const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(1);
}

int calc_state(uint32_t val)
{
  long step = (upper - lower)/nstates;
  if (val < lower + step/2) return 0;
  if (val > upper - step/2) return nstates-1;
  return 1 + (val - lower - step/2)/step;
}

static void cleanup()
{
  remeasure_deinit();
}

static void calibration_finished()
{
  report_error("Calibration finished lower: %i upper: %i\n", lower, upper);
  cleanup();
}

void calibrate()
{
  signal(SIGINT, &calibration_finished);
  lower = upper = -1;
  while (1)
  {
    remeasure_do();
    uint32_t val = remeasure_ask();
    if (val != -1)
    {
      if (lower == -1 || lower > val) lower = val;
      if (upper == -1 || upper < val) upper = val;
    }
    printf("%u\n", val);
    usleep(interval * 1000);
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
    if (!use_exec)
      execl("/bin/sh", "/bin/sh", handler, prev_str, cur_str, NULL);
    else
      execl(handler, handler, prev_str, cur_str, NULL);
    exit(0);//just to be sure
  }
  last_state = cur;
}

void watch_finished()
{
  handle_state(-1, 1);
  cleanup();
}

void watch()
{
  signal(SIGCHLD, SIG_IGN);
  signal(SIGTERM, &watch_finished);
  while (1)
  {
    remeasure_do();
    uint32_t val = remeasure_ask();
    if (val != -1)
    {
      int state = calc_state(val);
      printf("state: %i, val: %u\n", state, val);
      handle_state(state, 0);
    }
    else
    {
      printf("state: bad, val: %u\n", val);
    }
    usleep(interval*1000);
  }
}

int main(int argc, char **argv)
{
  int c;
  while ((c = getopt(argc, argv, "l:u:i:ch:n:e")) != -1)
  {
    long val;
    if (c != '?' && c != 'c' && c != 'h' && c != 'e')
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
    case 'e':
      use_exec = 1;
      break;
    default:
      report_error("Unknown argument %c\n", c == '?' ? optopt : c);
    }
  }
  if (interval < 0) report_error("Interval is not specified\n");
  if (!cal && lower < 0) report_error("Lower bound is not specified\n");
  if (!cal && upper < 0) report_error("Upper bound is not specified\n");
  if (!cal && !handler) report_error("Handler is not specified\n");
  if (!cal && nstates <= 0) report_error("Number of states is not specified\n");
  printf("lower: %u, upper: %u, nstates: %i\n", lower, upper, nstates);

  if (!remeasure_init()) report_error("Initialization failed\n");

  if (cal)
    calibrate(interval);
  else
    watch();
  report_error("Finished\n");
  return 0;
}
