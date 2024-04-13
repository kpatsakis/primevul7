static int is_ipmb_bcast_addr(struct ipmi_addr *addr)
{
	return addr->addr_type == IPMI_IPMB_BROADCAST_ADDR_TYPE;
}