#include <stdlib.h>
#include <stdio.h>

#include <netlink/netlink_snl_generic.h>

#define FAMILY_NAME "nl_echo"

int main()
{
	struct snl_state ss;

	if (!snl_init(&ss, NETLINK_GENERIC)) {
		printf("[NL_ECHO] Init error.\n");
		return (1);
	}

	uint16_t family_id = snl_get_genl_family(&ss, FAMILY_NAME);
	if (family_id == 0) {
		printf("[NL_ECHO] Family '%s' not found.\n", FAMILY_NAME);
		snl_free(&ss);
		return (1);
	}

	printf("[NL_ECHO] Family ID: %u\n", family_id);

	snl_free(&ss);
	return (0);
}
