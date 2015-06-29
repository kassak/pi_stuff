#ifndef REMEASURE_NETLINK_H_
#define REMEASURE_NETLINK_H_

#pragma push_macro("PROTECT")
#pragma push_macro("KERNEL")
#pragma push_macro("SELECT")

#define PROTECT(args...) args
#ifdef __KERNEL__
#  define KERNEL
#  define SELECT(K, U) PROTECT(PROTECT K)
#else
#  define SELECT(K, U) PROTECT(PROTECT U)
#endif

#ifdef KERNEL
#  include <net/genetlink.h>
#else
#  include <netlink/genl/genl.h>
#  include <netlink/genl/mngt.h>
#endif

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
  [REMEASURE_ATTR_A] = { .type = SELECT((NLA_NUL_STRING),(NLA_STRING)) },
};
enum { REMEASURE_VERSION = 1 };
static struct SELECT((genl_ops), (genl_cmd)) REMEASURE_OPS[] = {
  {
    SELECT((.cmd), (.c_id)) = REMEASURE_GET,
    SELECT((
      .flags = 0,
      .policy = REMEASURE_ATTR_POLICY,
      .doit = remeasure_get,
      .dumpit = NULL
    ),(
      .c_name = "REMEASURE_GET",
      .c_maxattr = REMEASURE_ATTR_END - 1,
      .c_msg_parser = remeasure_get_parser,
      .c_attr_policy = REMEASURE_ATTR_POLICY
    )),
  },
  {
    SELECT((.cmd), (.c_id)) = REMEASURE_POST,
    SELECT((
      .flags = 0,
      .policy = REMEASURE_ATTR_POLICY,
      .doit = remeasure_post,
      .dumpit = NULL
    ),(
      .c_name = "REMEASURE_POST",
      .c_maxattr = REMEASURE_ATTR_END - 1,
      .c_msg_parser = remeasure_post_parser,
      .c_attr_policy = REMEASURE_ATTR_POLICY
    )),
  }
};

static struct SELECT((genl_family), (genl_ops)) REMEASURE_FAMILY = {
  SELECT((.name),(.o_name)) = "REMEASURE",
  SELECT((
    .id = GENL_ID_GENERATE,
    .hdrsize = 0,
    .version = REMEASURE_VERSION,
    .maxattr = REMEASURE_ATTR_END - 1
  ),(
    .o_ncmds = REMEASURE_END,
    .o_cmds = REMEASURE_OPS
  ))
};

#pragma pop_macro("PROTECT")
#pragma pop_macro("KERNEL")
#pragma pop_macro("SELECT")

#endif// REMEASURE_NETLINK_H_
