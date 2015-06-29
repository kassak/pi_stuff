#ifndef REMEASURE_CLIENT_H_
#define REMEASURE_CLIENT_H_

#  include <netlink/genl/mngt.h>

static int remeasure_get_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg);
static int remeasure_post_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg);

#include "remeasure-netlink.h"

static int remeasure_get_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg)
{
  return 0;
}

static int remeasure_post_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg)
{
  return 0;
}

#endif// REMEASURE_CLIENT_H_
