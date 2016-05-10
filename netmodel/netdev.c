#include <stdlib.h>
#include <string.h>
#include "netdev.h"
#include "internal.h"
#include "tc.h"
#include "rule.h"
#include "util.h"

struct lsdn_netdev {
	struct lsdn_node node;
	struct lsdn_port port;
	struct lsdn_ruleset rules;
	struct lsdn_rule default_rule;
	char *linux_if;
};

struct lsdn_netdev *lsdn_netdev_new(
		struct lsdn_network *net,
		const char *linux_if)
{
	char *ifname_copy = strdup(linux_if);
	if(!ifname_copy)
		return NULL;

	struct lsdn_netdev *netdev = lsdn_as_netdev(
			lsdn_node_new(net, &lsdn_netdev_ops, sizeof(*netdev)));
	if(!netdev) {
		free(ifname_copy);
		return NULL;
	}
	netdev->linux_if = ifname_copy;
	netdev->node.port_count = 1;
	lsdn_port_init(&netdev->port, &netdev->node, 0, &netdev->rules);
	lsdn_ruleset_init(&netdev->rules);
	lsdn_rule_init(&netdev->default_rule);
	lsdn_add_rule(&netdev->rules, &netdev->default_rule, 0);
	netdev->default_rule.action.id = LSDN_ACTION_IF;
	netdev->default_rule.action.ifname = netdev->linux_if;

	lsdn_commit_to_network(&netdev->node);
	return netdev;
}

static struct lsdn_port *get_netdev_port(struct lsdn_node *node, size_t index)
{
	UNUSED(index);
	return &lsdn_as_netdev(node)->port;
}

static void free_netdev(struct lsdn_node *node)
{
	free(lsdn_as_netdev(node)->linux_if);
}

static lsdn_err_t update_if_rules(struct lsdn_node *node)
{
	// TODO: delete the old rules /  do not duplicate the rules

	struct lsdn_netdev *netdev = lsdn_as_netdev(node);
	runcmd("tc qdisc add dev %s handle ffff: ingress", netdev->linux_if);
	runcmd("tc filter add dev %s parent ffff: protocol all u32 match "
	       "u32 0 0 action mirred egress redirect dev %s",
	       netdev->linux_if, netdev->port.peer->ruleset->interface->ifname);
	return LSDNE_OK;
}

struct lsdn_node_ops lsdn_netdev_ops = {
	.free_private_data = free_netdev,
	.get_port = get_netdev_port,
	.update_rules = lsdn_noop,
	.update_ifs = lsdn_noop,
	.update_if_rules = update_if_rules
};
