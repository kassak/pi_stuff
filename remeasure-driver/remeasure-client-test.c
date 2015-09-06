#include "remeasure-client.h"
#include <stdio.h>
#include <unistd.h>

#define LOG(fmt, args...) fprintf(stderr, fmt, ##args)

int main(int argc, char *argv[])
{
  if (!remeasure_init())
  {
    LOG("init failed\n");
    return 1;
  }
  while (!remeasure_is_error_set())
  {
    if(!remeasure_do()) break;
    LOG("measure %u\n", remeasure_ask());
    sleep(1);
  }
  remeasure_deinit();
  return 0;
}
