#include <net/genetlink.h>

static int remeasure_get(struct sk_buff *skb, struct genl_info *info);
static int remeasure_post(struct sk_buff *skb, struct genl_info *info);

#include "remeasure-netlink.h"

static int remeasure_get(struct sk_buff *skb, struct genl_info *info)
{
  DKLOG("remeasure_get");
    /* message handling code goes here; return 0 on success, negative
     * values on failure */
  return 0;
}

static int remeasure_post(struct sk_buff *skb, struct genl_info *info)
{
  DKLOG("remeasure_post");
    /* message handling code goes here; return 0 on success, negative
     * values on failure */
  return 0;
}

int init_netlink(void)
{
  int res;
  if ((res = genl_register_family_with_ops(&REMEASURE_FAMILY, REMEASURE_OPS))) goto error0;
  DKLOG("netlink family %i registered", REMEASURE_FAMILY.id);
  return 0;
error0:
  return res;
}

void deinit_netlink(void)
{
  genl_unregister_family(&REMEASURE_FAMILY);
  DKLOG("netlink family %i unregistered", REMEASURE_FAMILY.id);
}
