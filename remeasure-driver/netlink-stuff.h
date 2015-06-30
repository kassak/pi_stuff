#include <net/genetlink.h>

static int remeasure_get(struct sk_buff *skb, struct genl_info *info);
static int remeasure_post(struct sk_buff *skb, struct genl_info *info);
static int remeasure_take(struct sk_buff *skb, struct genl_info *info);

#include "remeasure-netlink.h"

static int send_measure(struct genl_info *info)
{
  int res = 0;
  struct sk_buff * skb;
  void * msg_head;
  skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (!skb)
  {
    KLOG(KERN_ERR, "failed to create new message\n");
    res = -ENOMEM;
    goto error0;
  }
  msg_head = genlmsg_put(skb, 0, info->snd_seq, &REMEASURE_FAMILY, 0, REMEASURE_TAKE);
  if (!skb)
  {
    KLOG(KERN_ERR, "failed to set message type\n");
    res = -ENOMEM;
    goto error1;
  }
  if ((res = nla_put_u32(skb, REMEASURE_ATTR_VALUE, get_current_measure())))
  {
    KLOG(KERN_ERR, "failed to put measure to message\n");
    goto error1;
  }
  genlmsg_end(skb, msg_head);
  genlmsg_unicast(genl_info_net(info), skb, info->snd_portid);
  DKLOG("measure sent\n");
error1:
  if (res) nlmsg_free(skb);
error0:
  return res;
}

static int remeasure_take(struct sk_buff *skb, struct genl_info *info)
{
  DKLOG("remeasure_take");
  return 0;
}

static int remeasure_get(struct sk_buff *skb, struct genl_info *info)
{
  DKLOG("remeasure_get");
  return send_measure(info);
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
