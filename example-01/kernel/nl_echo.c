#include <sys/types.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/malloc.h>

#include <netlink/netlink.h>
#include <netlink/netlink_ctl.h>

#define ECHO_FAMILY_NAME    "nl_echo"
#define ECHO_FAMILY_VERSION 1

static uint16_t family_id = 0;

static int
nl_echo_loader(struct module *m, int ev, void *arg)
{
	int err = 0;

	switch (ev) {
	case MOD_LOAD:
		printf("[NL_ECHO] Generic netlink loaded.\n");
		family_id = genl_register_family(ECHO_FAMILY_NAME, 0, ECHO_FAMILY_VERSION, 0);
		if (family_id == 0) {
			printf("[NL_ECHO] Erro registering the family\n");
			return (EINVAL);
		}
		break;
	case MOD_UNLOAD:
		printf("[NL_ECHO] Generic netlink unloaded.\n");
		genl_unregister_family(family_id);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	return (err);
}

static moduledata_t nl_echo_mod = {
	"nl_echo",
	nl_echo_loader,
	NULL
};

DECLARE_MODULE(nl_echo, nl_echo_mod, SI_SUB_PSEUDO, SI_ORDER_ANY);
