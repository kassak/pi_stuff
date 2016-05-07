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
  REMEASURE_TAKE,

  REMEASURE_END
};
enum
{
  REMEASURE_ATTR_VALUE = 1,

  REMEASURE_ATTR_END
};
static struct nla_policy REMEASURE_ATTR_POLICY[REMEASURE_ATTR_END] = {
  [REMEASURE_ATTR_VALUE] = { .type = NLA_U32 },
};
enum { REMEASURE_VERSION = 1 };
#pragma push_macro("OPERATION")
#define OPERATION(ID, DO, PARSER) {        \
  SELECT((                                 \
    .cmd = ID,                             \
    .flags = 0,                            \
    .policy = REMEASURE_ATTR_POLICY,       \
    .doit = DO,                            \
    .dumpit = NULL                         \
  ),(                                      \
    .c_id = ID,                            \
    .c_name = #ID,                         \
    .c_maxattr = REMEASURE_ATTR_END - 1,   \
    .c_msg_parser = PARSER,                \
    .c_attr_policy = REMEASURE_ATTR_POLICY \
  )),                                      \
}
static struct SELECT((genl_ops), (genl_cmd)) REMEASURE_OPS[] = {
  OPERATION(REMEASURE_GET,  remeasure_get,  NULL),
  OPERATION(REMEASURE_POST, remeasure_post, NULL),
  OPERATION(REMEASURE_TAKE, remeasure_take, remeasure_take_parser),
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
