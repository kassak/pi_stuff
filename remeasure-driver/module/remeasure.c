#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int gpio_a_pin = -1, gpio_b_pin = -1;
int measure_timeout_usec = -1, min_relaxation_usec = 0;
bool debug = 0;
char gpiochip_name[30] = "pinctrl-bcm2835";

struct gpio_chip *gpiochip;
struct irq_data *irqdata;
DEFINE_SPINLOCK(irqlock);

uint32_t current_measure = -1;

uint32_t get_current_measure(void)
{
    //TODO: lock
    return current_measure;
}

#include "remeasure-specification.h"
#include "util.h"
#include "gpio-stuff.h"
#include "measure.h"
#include "netlink-stuff.h"

static inline int init_gpio(void)
{
  int ret;
  gpiochip = find_gpio_chip(gpiochip_name);
  if (!gpiochip) return -ENODEV;
  if ((ret = claim_pin(gpiochip, gpio_a_pin))) goto error0;
  if ((ret = claim_pin(gpiochip, gpio_b_pin))) goto error1;
  if ((ret = install_irq_handler(gpiochip, gpio_b_pin, (irq_handler_t)measure_irq_handler
                                 , MY_MODULE_NAME " charge waiter", &irqdata)))
    goto error2;
  set_irq_type(irqdata, IRQ_TYPE_EDGE_RISING, &irqlock);
  on_irq(irqdata, &irqlock);
  return 0;
error2:
  free_pin(gpiochip, gpio_b_pin);
error1:
  free_pin(gpiochip, gpio_a_pin);
error0:
  return ret;
}

static inline void deinit_gpio(void)
{
  off_irq(irqdata, &irqlock);
  set_irq_type(irqdata, 0, &irqlock);
  uninstall_irq_handler(gpiochip, gpio_b_pin);
  free_pin(gpiochip, gpio_a_pin);
  free_pin(gpiochip, gpio_b_pin);
}

static int remeasure_init(void)
{
  int res;
  KLOG(KERN_INFO, "inited on A=%i, B=%i, timeout=%i, relaxation=%i\n",
      gpio_a_pin, gpio_b_pin, measure_timeout_usec, min_relaxation_usec);
  if ((res = init_gpio())) goto error0;
  if ((res = init_netlink())) goto error1;
  if ((res = init_measurements())) goto error2;
  return 0;
error2:
  deinit_measurements();
error1:
  deinit_gpio();
error0:
  return res;
}

static void remeasure_deinit(void)
{
  deinit_gpio();
  deinit_netlink();
  deinit_measurements();
  KLOG(KERN_INFO, "deinited\n");
}

module_init(remeasure_init);
module_exit(remeasure_deinit);
