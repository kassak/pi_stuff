#define OUT_PREFIX MODULE_NAME ": "
#define KLOG(lvl, fmt, args...) printk(lvl OUT_PREFIX fmt, ## args)
#define DKLOG(fmt, args...) do { if (debug) KLOG(KERN_DEBUG, fmt, ## args); } while (0)
