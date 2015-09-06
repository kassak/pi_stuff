#include <linux/timer.h>
#include <linux/time.h>

enum {
  Q_FREE, Q_WAIT, Q_MEASURE
};

int measure_state = Q_FREE;
int last_measure_jif = 0;
struct timeval measure_start_time;
static struct timer_list measure_timer, dequeue_timer;

static void set_gpio_charging(void)
{
  set_output(gpiochip, gpio_a_pin, 1);
  set_input(gpiochip, gpio_b_pin);
}

static void set_gpio_discharging(void)
{
  set_input(gpiochip, gpio_a_pin);
  set_output(gpiochip, gpio_b_pin, 0);
}

static int set_measure(uint32_t measure)
{
  if (measure_state != Q_FREE)
  {
    off_irq(irqdata, &irqlock);
    current_measure = measure;
    measure_state = Q_FREE;
    set_gpio_discharging();
  }
  return 0;
}

static int start_measure_impl(void)
{
  int timeout_jif, res = 0;
  DKLOG("start_measure_impl\n");
  measure_state = Q_MEASURE;
  on_irq(irqdata, &irqlock);
  do_gettimeofday(&measure_start_time);
  set_gpio_charging();
  if (measure_timeout_usec != -1)
  {
    DKLOG("setting timeout guard\n");
    timeout_jif = usecs_to_jiffies(measure_timeout_usec) + jiffies;
    if ((res = mod_timer(&measure_timer, timeout_jif)))
      KLOG(KERN_ERR, "failed to set timeout guard with timeout %i\n", jiffies_to_usecs(timeout_jif));
  }
  return res;
}

static irqreturn_t measure_irq_handler(int i, void *blah, struct pt_regs *regs)
{
  unsigned long flags;
  struct timeval stop_time;
  int period;
  spin_lock_irqsave(&irqlock, flags);
  do_gettimeofday(&stop_time);
  period = stop_time.tv_sec - measure_start_time.tv_sec;
  period *= 1000000;
  period += stop_time.tv_usec - measure_start_time.tv_usec;
  set_measure(period);
  spin_unlock_irqrestore(&irqlock, flags);
  return IRQ_HANDLED;
}

static void kill_measure(unsigned long data)
{
  unsigned long flags;
  spin_lock_irqsave(&irqlock, flags);
  set_measure(-1);
  spin_unlock_irqrestore(&irqlock, flags);
}

static void start_measure(unsigned long data)
{
  unsigned long flags;
  DKLOG("start_measure\n");
  spin_lock_irqsave(&irqlock, flags);
  start_measure_impl();
  spin_unlock_irqrestore(&irqlock, flags);
}

static int enqueue_measure(void)
{
  unsigned long flags;
  int res = 0;
  int timeout_jif;
  DKLOG("enqueue_measure\n");
  spin_lock_irqsave(&irqlock, flags);
  if (measure_state == Q_FREE)
  {
    measure_state = Q_WAIT;
    timeout_jif = usecs_to_jiffies(min_relaxation_usec) - (jiffies - last_measure_jif);
    if (timeout_jif <= 0)
    {
      if ((res = start_measure_impl()))
        KLOG(KERN_ERR, "failed to start measure directly\n");
    }
    else
    {
      if ((res = mod_timer(&dequeue_timer, jiffies + timeout_jif)))
        KLOG(KERN_ERR, "failed to start measure with timeout %i\n", jiffies_to_usecs(timeout_jif));
    }
  }
  spin_unlock_irqrestore(&irqlock, flags);
  return res;
}

static int init_measurements(void)
{
  setup_timer(&measure_timer, kill_measure, 0);
  setup_timer(&dequeue_timer, start_measure, 0);
  return 0;
}

static int deinit_measurements(void)
{
  del_timer(&measure_timer);
  del_timer(&dequeue_timer);
  return 0;
}
