#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int gpio_a_pin = -1, gpio_b_pin = -1;
int measure_timeout = -1, min_relaxation_ms = 0;
bool debug = 0;
char gpiochip_name[30] = "pinctrl-bcm2835";

struct gpio_chip *gpiochip;
struct irq_data *irqdata;
static spinlock_t lock;

#include "remeasure_specification.h"
#include "util.h"
#include "gpio_stuff.h"

static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
{

    /* use the GPIO signal level */
    /*int signal = */gpiochip->get(gpiochip, gpio_a_pin);

    /* unmask the irq */
    //irqchip->irq_unmask(irqdata);


    return IRQ_HANDLED;
}


static inline int init_gpio(void)
{
  int ret;
  gpiochip = find_gpio_chip(gpiochip_name);
  if (!gpiochip) return -ENODEV;
  if ((ret = claim_pin(gpiochip, gpio_a_pin))) goto error0;
  if ((ret = claim_pin(gpiochip, gpio_b_pin))) goto error1;
  if ((ret = install_irq_handler(gpiochip, gpio_b_pin, irq_handler
                                 , MY_MODULE_NAME " charge waiter", &irqdata)))
    goto error2;
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
  uninstall_irq_handler(gpiochip, gpio_b_pin);
  free_pin(gpiochip, gpio_a_pin);
  free_pin(gpiochip, gpio_b_pin);
}

static int remeasure_init(void)
{
  int res;
  KLOG(KERN_INFO, "inited\n");
  DKLOG("on A=%i, B=%i, timeout=%i, relaxation=%i\n",
      gpio_a_pin, gpio_b_pin, measure_timeout, min_relaxation_ms);
  if ((res = init_gpio())) return res;
  return 0;
}

static void remeasure_deinit(void)
{
  deinit_gpio();
  KLOG(KERN_INFO, "deinited\n");
}

module_init(remeasure_init);
module_exit(remeasure_deinit);
