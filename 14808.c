static int ax_close(struct net_device *dev)
{
	struct mkiss *ax = netdev_priv(dev);

	if (ax->tty)
		clear_bit(TTY_DO_WRITE_WAKEUP, &ax->tty->flags);

	netif_stop_queue(dev);

	return 0;
}