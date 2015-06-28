#include <net/genetlink.h>

static int remeasure_get(struct sk_buff *skb, struct genl_info *info);
static int remeasure_post(struct sk_buff *skb, struct genl_info *info);

enum
{
  REMEASURE_GET = 1,
  REMEASURE_POST,

  REMEASURE_END
};
enum
{
  REMEASURE_ATTR_A = 1,

  REMEASURE_ATTR_END
};
static struct nla_policy REMEASURE_ATTR_POLICY[REMEASURE_ATTR_END] = {
    [REMEASURE_ATTR_A] = { .type = NLA_NUL_STRING },
};
enum { REMEASURE_VERSION = 1 };
static struct genl_family REMEASURE_FAMILY = {
  .id = GENL_ID_GENERATE,
  .hdrsize = 0,
  .name = "REMEASURE",
  .version = REMEASURE_VERSION,
  .maxattr = REMEASURE_ATTR_END - 1
};
static struct genl_ops REMEASURE_OPS[] = {
  {
    .cmd = REMEASURE_GET,
    .flags = 0,
    .policy = REMEASURE_ATTR_POLICY,
    .doit = remeasure_get,
    .dumpit = NULL,
  },
  {
    .cmd = REMEASURE_POST,
    .flags = 0,
    .policy = REMEASURE_ATTR_POLICY,
    .doit = remeasure_post,
    .dumpit = NULL,
  }
};
static int remeasure_get(struct sk_buff *skb, struct genl_info *info)
{
    /* message handling code goes here; return 0 on success, negative
     * values on failure */
  return 0;
}

static int remeasure_post(struct sk_buff *skb, struct genl_info *info)
{
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
