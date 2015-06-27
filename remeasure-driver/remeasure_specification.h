#define MODULE_NAME "Remeasure"
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Resistance measure driver for Raspberry Pi GPIO.");
MODULE_AUTHOR("Alexander Kass <kass.soft@gmail.com>");

module_param(gpio_b_pin, int, S_IRUGO);
MODULE_PARM_DESC(gpio_b_pin, "GPIO pin nearest to capacitor."
                 "Valid pin numbers are: 0, 1, 4, 8, 7, 9, 10, 11,"
                 "14, 15, 17, 18, 21, 22, 23, 24, 25. Default is none");

module_param(gpio_a_pin, int, S_IRUGO);
MODULE_PARM_DESC(gpio_a_pin, "GPIO pin connected to capacitor through measured resistance."
                 " Valid pin numbers are: 0, 1, 4, 8, 7, 9, 10, 11, 14, 15,"
                 " 17, 18, 21, 22, 23, 24, 25. Default is none");

module_param(measure_timeout, int, S_IRUGO);
MODULE_PARM_DESC(measure_timeout, "Maximum charge time (ms)."
                 " If charges more measure considered invalid. Default is infinity.");

module_param(min_relaxation_ms, int, S_IRUGO);
MODULE_PARM_DESC(gpio_a_pin, "Minimal discharge time (ms)."
                 " Measures on charged capacitor are denied or previous measure returned. Default is 0");

module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable debugging messages");
