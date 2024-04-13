static int is_lan_addr(struct ipmi_addr *addr)
{
	return addr->addr_type == IPMI_LAN_ADDR_TYPE;
}