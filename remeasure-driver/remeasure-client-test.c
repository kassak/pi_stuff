#include "remeasure-client.h"

int main(int argc, char *argv[])
{
    int res;
    if ((res = genl_register_family(&REMEASURE_FAMILY)) < 0) return res;

    genl_unregister_family(&REMEASURE_FAMILY);

    return 0;
}
