#include <stdlib.h>
#include <stdio.h>

#include <netlink/netlink_snl_generic.h>

#define FAMILY_NAME "nl_echo"

/*
 * Commands  (TODO: share it with kernel)
 */
enum {
	ECHO_CMD_UNSPEC = 0,
	ECHO_CMD_REPLY  = 1,
	__ECHO_CMD_MAX,
};
#define ECHO_CMD_MAX (__ECHO_CMD_MAX - 1)

/*
 * Attributes (TODO: share it with kernel)
 */
enum {
	ECHO_ATTR_UNSPEC = 0,
	ECHO_ATTR_VALUE  = 1,
	__ECHO_ATTR_MAX,
};
#define ECHO_ATTR_MAX (__ECHO_ATTR_MAX - 1)

/* Echo state */
struct nl_echo_state {
	uint32_t value;
};

/* Attribute parser */
#define _OUT(_field)	offsetof(struct nl_echo_state, _field)
static const struct snl_attr_parser nla_p_state[] = {
	{ .type = ECHO_ATTR_VALUE, .off = _OUT(value), .cb = snl_attr_get_uint32 },
};
#undef _OUT

/* Declare generic netlink parser 'nl_echo_state_parser' */
SNL_DECLARE_GENL_PARSER(nl_echo_state_parser, nla_p_state);

int main()
{
	struct snl_state ss;
	struct snl_writer nw;
	struct nlmsghdr *hdr;

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

	/* Prepare the request
	 *
	 * Note: snl_create_genl_msg_request does not set the family version in
	 * the generic netlink header, i.e., version = 0. If the kernel checks the
	 * family version for compatibility, this could be a problem.
	 */
	snl_init_writer(&ss, &nw);
	hdr = snl_create_genl_msg_request(&nw, family_id, ECHO_CMD_REPLY);
	if (hdr == NULL) {
		printf("[NL_ECHO] Create request error\n");
		snl_free(&ss);
		return (1);
	}

	if (!snl_add_msg_attr_u32(&nw, ECHO_ATTR_VALUE, 42)) {
		printf("[NL_ECHO] Create request error\n");
		snl_free(&ss);
		return (1);
	}

	/* Send the message */
	if ((hdr = snl_finalize_msg(&nw)) == NULL || !snl_send_message(&ss, hdr)) {
		printf("[NL_ECHO] Send message error\n");
		snl_free(&ss);
		return (1);
	}

	/* Receive the reply from the sequence number */
	hdr = snl_read_reply(&ss, hdr->nlmsg_seq);
	if (hdr == NULL || hdr->nlmsg_type == NLMSG_ERROR) {
		printf("[NL_ECHO] Reply error\n");
		snl_free(&ss);
		return (1);
	}

	printf("[NL_ECHO] Reply received\n");

	/* Parse the message */
	struct nl_echo_state attrs = {};
	if (!snl_parse_nlmsg(&ss, hdr, &nl_echo_state_parser, &attrs)) {
		printf("[NL_ECHO] Parser error\n");
		snl_free(&ss);
		return (1);
	}

	printf("[NL_ECHO] Reply: %u\n", attrs.value);

	snl_free(&ss);
	return (0);
}
