#include "remeasure-client.h"

#include <netlink/genl/mngt.h>

static struct nl_sock * sock;
static int last_nl_err = 0;
static uint32_t last_measure;

static int remeasure_take_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg);

#include "../module/remeasure-netlink.h"

static int remeasure_take_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg)
{
  if (!info->attrs[REMEASURE_ATTR_VALUE]) return -1;
  last_measure = *(uint32_t*)nla_data(info->attrs[REMEASURE_ATTR_VALUE]);
  return 0;
}

int remeasure_init(void)
{
  if (!(sock = nl_socket_alloc())) return 0;
  if ((last_nl_err = genl_register_family(&REMEASURE_FAMILY))) goto error0;
  if ((last_nl_err = genl_connect(sock))) goto error1;
  if ((last_nl_err = genl_mngt_resolve(sock))) goto error2;
  if ((last_nl_err = nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, &genl_handle_msg, NULL))) goto error2;
  return 1;
error2:
  nl_close(sock);
error1:
  genl_unregister_family(&REMEASURE_FAMILY);
error0:
  nl_socket_free(sock);
  return 0;
}
int remeasure_do(void)
{
  last_nl_err = genl_send_simple(sock, REMEASURE_FAMILY.o_id, REMEASURE_POST, REMEASURE_VERSION, 0);
  if (last_nl_err < 0) return 0;
  if ((last_nl_err = nl_wait_for_ack(sock))) return 0;
  return 1;
}
uint32_t remeasure_last(void)
{
  return last_measure;
}
uint32_t remeasure_ask(void)
{
  last_nl_err = genl_send_simple(sock, REMEASURE_FAMILY.o_id, REMEASURE_GET, REMEASURE_VERSION, 0);
  if (last_nl_err < 0) return -1;

  if ((last_nl_err = nl_wait_for_ack(sock))) return -1;
  if ((last_nl_err = nl_recvmsgs_default(sock))) return -1;
  return last_measure;
}

void remeasure_deinit(void)
{
  nl_close(sock);
  genl_unregister_family(&REMEASURE_FAMILY);
  nl_socket_free(sock);
}

int remeasure_is_error_set(void)
{
  return last_nl_err < 0;
}
