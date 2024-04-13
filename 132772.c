static int check_addr(struct ipmi_smi  *intf,
		      struct ipmi_addr *addr,
		      unsigned char    *saddr,
		      unsigned char    *lun)
{
	if (addr->channel >= IPMI_MAX_CHANNELS)
		return -EINVAL;
	addr->channel = array_index_nospec(addr->channel, IPMI_MAX_CHANNELS);
	*lun = intf->addrinfo[addr->channel].lun;
	*saddr = intf->addrinfo[addr->channel].address;
	return 0;
}