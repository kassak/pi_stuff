#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

static inline void set_level(struct gpio_chip * chip, int pin, int value)
{
  chip->set(chip, pin, value);
}

static inline int get_level(struct gpio_chip * chip, int pin)
{
  return chip->get(chip, pin);
}

static inline void set_output(struct gpio_chip * chip, int pin, int val)
{
  chip->direction_output(chip, pin, val);
}

static inline void set_input(struct gpio_chip * chip, int pin)
{
  chip->direction_input(chip, pin);
}

static int is_right_chip(struct gpio_chip *chip, void *data)
{
  int res = strcmp(data, chip->label) == 0;
  DKLOG("is_right_chip %s %d\n", chip->label, res);
  return res;
}

static inline int claim_pin(struct gpio_chip * chip, int pin)
{
  if (pin < 0 || pin >= chip->ngpio)
  {
    KLOG(KERN_ERR, "GPIO pin %i out of possible range 0..%i\n", pin, gpiochip->ngpio);
    return -EINVAL;
  }
  if (chip->request(chip, pin))
  {
    KLOG(KERN_ERR, "failed to claim GPIO pin %i", pin);
    return -ENODEV;
  }
  DKLOG("successfully claimed GPIO pin %i (%s)", pin
        , chip->names && chip->names[pin] ? chip->names[pin] : "noname");
  return 0;
}

static inline void free_pin(struct gpio_chip * chip, int pin)
{
  chip->free(chip, pin);
}

static inline struct gpio_chip* find_gpio_chip(const char * name)
{
  return gpiochip_find((void*)name, is_right_chip);
}

static inline int install_irq_handler(struct gpio_chip * chip, int pin, irq_handler_t handler
                                      , const char * name, struct irq_data **res)
{
  int result;
  int irq = chip->to_irq(chip, pin);
  struct irq_data *data = irq_get_irq_data(irq);
  DKLOG("irq %d\n", irq);

  if (!data || !data->chip)
  {
    KLOG(KERN_ERR, "no irq chip found");
    return -ENODEV;
  }
  result = request_irq(irq, handler, 0, name, (void*) 0);
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
  if (res != NULL) *res = data;
  return result;
}

static void uninstall_irq_handler(struct gpio_chip * chip, int pin)
{
  int irq = chip->to_irq(chip, pin);
  free_irq(irq, (void *) 0);
  DKLOG("freed IRQ %d\n", irq);
}

static inline void set_irq_type(struct irq_data * data, int type, spinlock_t * lock)
{
  unsigned long flags;
  DKLOG("set_irq_type %i -> %i\n", data->irq, type);
  spin_lock_irqsave(lock, flags);
  data->chip->irq_set_type(data, type);
  spin_unlock_irqrestore(lock, flags);
}

static inline void unmask_irq(struct irq_data * data, spinlock_t * lock)
{
  unsigned long flags;
  DKLOG("unmask_irq %i\n", data->irq);
  spin_lock_irqsave(lock, flags);
  data->chip->irq_unmask(data);
  spin_unlock_irqrestore(lock, flags);
}

static inline void mask_irq(struct irq_data * data, spinlock_t * lock)
{
  unsigned long flags;
  DKLOG("mask_irq %i\n", data->irq);
  spin_lock_irqsave(lock, flags);
  data->chip->irq_mask(data);
  spin_unlock_irqrestore(lock, flags);
}
