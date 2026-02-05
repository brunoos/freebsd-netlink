#include <sys/types.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/malloc.h>

#include <netlink/netlink.h>
#include <netlink/netlink_ctl.h>
#include <netlink/netlink_generic.h>
#include <netlink/netlink_message_writer.h>

#define ECHO_FAMILY_NAME    "nl_echo"
#define ECHO_FAMILY_VERSION 1

/* Commands */
enum {
	ECHO_CMD_UNSPEC = 0,
	ECHO_CMD_REPLY  = 1,
	__ECHO_CMD_MAX,
};
#define ECHO_CMD_MAX (__ECHO_CMD_MAX - 1)

/* Attribute */
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
static const struct nlattr_parser nla_p_state[] = {
	{ .type = ECHO_ATTR_VALUE, .off = _OUT(value), .cb = nlattr_get_uint32 },
};
#undef _OUT

/* Declare nl_echo_state_parser that does not parser the header fields,
 * only the attributes.
 */
NL_DECLARE_PARSER(nl_echo_state_parser, struct genlmsghdr, nlf_p_empty, nla_p_state);

/* Echo family id */
static uint16_t family_id = 0;

/*
 * Reply command handler
 */
static int
echo_cmd_reply_handler(struct nlmsghdr *hdr, struct nl_pstate *npt)
{
	printf("[NL_ECHO] Reply command handler\n");

	/* Parse the message */
	struct nl_echo_state attrs = {};
	if (nl_parse_nlmsg(hdr, &nl_echo_state_parser, npt, &attrs) != 0) {
		printf("[NL_ECHO] Parser error\n");
		return (EINVAL);
	}

	struct nl_writer *nw = npt->nw;

	/* Copy data from netlink request header to response header.
	 * Set the payload size as generic netlink header size.
	 */
	if (!nlmsg_reply(nw, hdr, sizeof(struct genlmsghdr))) {
		printf("[NL_ECHO] Reply error\n");
		nlmsg_abort(nw);
		return (ENOMEM);
	}

	/* Create the (zeroed) generic netlink header */
	struct genlmsghdr *ghdr = nlmsg_reserve_object(nw, struct genlmsghdr);
	if (ghdr == NULL) {
		printf("[NL_ECHO] Reply error\n");
		nlmsg_abort(nw);
		return (ENOMEM);
	}

	ghdr->cmd = ECHO_CMD_REPLY;
	ghdr->version = ECHO_FAMILY_VERSION;

	/* Reply the value */
	printf("[NL_ECHO] Reply: %u\n", attrs.value);
	if (nlattr_add_u32(nw, ECHO_ATTR_VALUE, attrs.value) == false) {
		printf("[NL_ECHO] Reply error\n");
		nlmsg_abort(nw);
		return (ENOMEM);
	}

	/* Finish the response */
	if (nlmsg_end(nw))
		return (0);

	/* Something goes wrong with the response */
	nlmsg_abort(nw);
	return (ENOMEM);
}

/* Register the callback and flags to the command */
static const struct genl_cmd echo_cmds[] = {
	{
		.cmd_num   = ECHO_CMD_REPLY,
		.cmd_name  = "REPLY",
		.cmd_cb    = echo_cmd_reply_handler,
		.cmd_flags = GENL_CMD_CAP_DO,
	}
};

/*
 * Echo generic netlink load/unload
 */
static int
nl_echo_loader(struct module *m, int ev, void *arg)
{
	int err = 0;

	switch (ev) {
	case MOD_LOAD:
		printf("[NL_ECHO] Generic netlink loaded.\n");
		family_id = genl_register_family(ECHO_FAMILY_NAME, 0, ECHO_FAMILY_VERSION, ECHO_ATTR_MAX);
		if (family_id == 0) {
			printf("[NL_ECHO] Family register error\n");
			return (EINVAL);
		}
		if (!genl_register_cmds(family_id, echo_cmds, nitems(echo_cmds))) {
			genl_unregister_family(family_id);
			family_id = 0;
			printf("[NL_ECHO] Commands register error\n");
			return (EINVAL);
		}
		break;
	case MOD_UNLOAD:
		printf("[NL_ECHO] Generic netlink unloaded.\n");
		genl_unregister_family(family_id);
		family_id = 0;
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
