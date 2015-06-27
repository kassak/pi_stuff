int valid_gpio_pins[] = {
  0, 1, 4, 8, 7, 9, 10, 11, 14, 15, 17, 18, 21, 22, 23, 24, 25
};

static bool is_valid_gpio_pin(int pin)
{
  int i;
  for (i = 0; i < ARRAY_SIZE(valid_gpio_pins); ++i)
    if (valid_gpio_pins[i] == pin) return true;
  return false;
}
