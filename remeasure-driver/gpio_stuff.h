#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

struct gpio_chip *gpiochip;
struct irq_chip *irqchip;
struct irq_data *irqdata;
static spinlock_t lock;

static inline void set_level(int pin, int value)
{
  gpiochip->set(gpiochip, pin, value);
}

static inline int get_level(int pin)
{
  return gpiochip->get(gpiochip, pin);
}

static inline void set_output(int pin, int val)
{
  gpiochip->direction_output(gpiochip, pin, val);
}

static inline void set_input(int pin)
{
  gpiochip->direction_input(gpiochip, pin);
}

static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
{

  /* use the GPIO signal level */
  /*int signal = */gpiochip->get(gpiochip, gpio_a_pin);

  /* unmask the irq */
  irqchip->irq_unmask(irqdata);


  return IRQ_HANDLED;
}

static int is_right_chip(struct gpio_chip *chip, void *data)
{
  int res = strcmp(data, chip->label) == 0;
  DKLOG("is_right_chip %s %d\n", chip->label, res);
  return res;
}

static inline int claim_pin(int pin, const char * name)
{
  if (gpio_request(pin, name))
  {
    KLOG(KERN_ERR, "failed to claim GPIO pin %i", pin);
    return -ENODEV;
  }
  return 0;
}

static inline int init_gpio(void)
{
  int ret, irq;
  gpiochip = gpiochip_find("bcm2708_gpio", is_right_chip);
  if (!gpiochip) return -ENODEV;

  if ((ret = claim_pin(gpio_a_pin, MY_MODULE_NAME "A"))) goto error0;
  if ((ret = claim_pin(gpio_b_pin, MY_MODULE_NAME "B"))) goto error1;

  irq = gpiochip->to_irq(gpiochip, gpio_b_pin);
  DKLOG("irq %d\n", irq);
  irqdata = irq_get_irq_data(irq);

  if (!irqdata || !irqdata->chip)
  {
    KLOG(KERN_ERR, "no irq chip found");
    ret = -ENODEV;
    goto error2;
  }
  irqchip = irqdata->chip;
  return 0;
error2:
  gpio_free(gpio_b_pin);
error1:
  gpio_free(gpio_a_pin);
error0:
  return ret;
}

static inline void deinit_gpio(void)
{
  gpio_free(gpio_a_pin);
  gpio_free(gpio_b_pin);
}

static int install_irq_handler(int pin, int irq_type, irq_handler_t handler, const char * name)
{
  unsigned long flags;
  int irq = gpiochip->to_irq(gpiochip, pin);
  int result = request_irq(irq, handler, 0, name, (void*) 0);

  switch (result)
  {
  case -EBUSY:
    KLOG(KERN_ERR, "IRQ %d is busy\n", irq);
    return -EBUSY;
  case -EINVAL:
    KLOG(KERN_ERR, "bad irq number or handler\n");
    return -EINVAL;
  default:
    DKLOG("interrupt %d obtained\n", irq);
    break;
  }

  spin_lock_irqsave(&lock, flags);
  irqchip->irq_set_type(irqdata, IRQ_TYPE_EDGE_RISING | IRQ_TYPE_EDGE_FALLING);
  irqchip->irq_unmask(irqdata);
  spin_unlock_irqrestore(&lock, flags);
  return 0;
}

static void uninstall_irq_handler(int pin)
{
  unsigned long flags;
  int irq;
  spin_lock_irqsave(&lock, flags);
  irqchip->irq_set_type(irqdata, 0);
  irqchip->irq_mask(irqdata);
  spin_unlock_irqrestore(&lock, flags);

  irq = gpiochip->to_irq(gpiochip, pin);
  free_irq(irq, (void *) 0);
  DKLOG("freed IRQ %d\n", irq);
}
