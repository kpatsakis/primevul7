int netdev_boot_setup_check(struct net_device *dev)
{
	struct netdev_boot_setup *s = dev_boot_setup;
	int i;

	for (i = 0; i < NETDEV_BOOT_SETUP_MAX; i++) {
		if (s[i].name[0] != '\0' && s[i].name[0] != ' ' &&
		    !strcmp(dev->name, s[i].name)) {
			dev->irq = s[i].map.irq;
			dev->base_addr = s[i].map.base_addr;
			dev->mem_start = s[i].map.mem_start;
			dev->mem_end = s[i].map.mem_end;
			return 1;
		}
	}
	return 0;
}