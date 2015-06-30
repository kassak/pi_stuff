#include "remeasure-client.h"
#include <stdio.h>
#include <unistd.h>

#define LOG(fmt, args...) fprintf(stderr, fmt, ##args)

static int parse_cb(struct nl_msg *msg, void *arg)
{
  return genl_handle_msg(msg, NULL);
}

int main(int argc, char *argv[])
{
  int res = 0;
  struct nl_sock * sock;
  LOG("allocating socket\n");
  if (!(sock = nl_socket_alloc())) return -1;
  LOG("registering family\n");
  if ((res = genl_register_family(&REMEASURE_FAMILY))) goto error0;
  LOG("connecting\n");
  if ((res = genl_connect(sock))) goto error1;
  LOG("resolving family\n");
  if ((res = genl_mngt_resolve(sock))) goto error2;
  LOG("setting callback\n");
  if ((res = nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, parse_cb, NULL))) goto error2;

  LOG("sending\n");
  res = genl_send_simple(sock, REMEASURE_FAMILY.o_id, REMEASURE_GET, REMEASURE_VERSION, 0);
  if ((res = res > 0 ? 0 : res)) goto error2; //libnl bug

  if ((res = nl_recvmsgs_default(sock))) goto error2;
error2:
  nl_close(sock);
error1:
  genl_unregister_family(&REMEASURE_FAMILY);
error0:
  nl_socket_free(sock);
  if (res) LOG("error: %s\n", nl_geterror(res));
  return res;
}
