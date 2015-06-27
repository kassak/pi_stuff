#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int gpio_a_pin = -1, gpio_b_pin = -1;
int measure_timeout = -1, min_relaxation_ms = 0;
bool debug = 0;

#include "remeasure_specification.h"
#include "util.h"
#include "gpio_stuff.h"

static int remeasure_init(void)
{
  KLOG(KERN_INFO, "inited\n");
  DKLOG("on A=%i, B=%i, timeout=%i, relaxation=%i\n",
      gpio_a_pin, gpio_b_pin, measure_timeout, min_relaxation_ms);
  if (!is_valid_gpio_pin(gpio_a_pin))
  {
    KLOG(KERN_ERR, "A pin is invalid\n");
    return -EINVAL;
  }
  if (!is_valid_gpio_pin(gpio_b_pin))
  {
      KLOG(KERN_ERR, "B pin is invalid\n");
      return -EINVAL;
  }
  return 0;
}

static void remeasure_deinit(void)
{
  KLOG(KERN_INFO, "deinited\n");
}

module_init(remeasure_init);
module_exit(remeasure_deinit);
