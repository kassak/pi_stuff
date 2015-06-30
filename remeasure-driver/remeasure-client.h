#ifndef REMEASURE_CLIENT_H_
#define REMEASURE_CLIENT_H_

#include <netlink/genl/mngt.h>
#include <stdio.h>

static int remeasure_take_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg);

#include "remeasure-netlink.h"

static int remeasure_take_parser(
  struct nl_cache_ops *unused, struct genl_cmd *cmd,
  struct genl_info *info, void *arg)
{
  if (!info->attrs[REMEASURE_ATTR_VALUE])
  {
    printf("no value\n");
    return -1;
  }
  uint32_t value = *(uint32_t*)nla_data(info->attrs[REMEASURE_ATTR_VALUE]);
  printf("received value %i\n", value);
  return 0;
}

#endif// REMEASURE_CLIENT_H_
