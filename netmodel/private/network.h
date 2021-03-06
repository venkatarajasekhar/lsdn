#ifndef _LSDN_NETWORK_H_PRIVATE_
#define _LSDN_NETWORK_H_PRIVATE_

#include "../include/network.h"
#include "list.h"

#define LSDN_MAX_IF_SUFFIX 20

struct lsdn_network {
	char *name;
	/* Last used unique id for interface names etc */
	int unique_id;
	struct lsdn_list_entry nodes;
};

#endif
