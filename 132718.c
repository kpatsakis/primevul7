static int is_ipmb_addr(struct ipmi_addr *addr)
{
	return addr->addr_type == IPMI_IPMB_ADDR_TYPE;
}