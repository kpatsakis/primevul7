static void mkiss_write_wakeup(struct tty_struct *tty)
{
	struct mkiss *ax = mkiss_get(tty);
	int actual;

	if (!ax)
		return;

	if (ax->xleft <= 0)  {
		/* Now serial buffer is almost free & we can start
		 * transmission of another packet
		 */
		clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);

		netif_wake_queue(ax->dev);
		goto out;
	}

	actual = tty->ops->write(tty, ax->xhead, ax->xleft);
	ax->xleft -= actual;
	ax->xhead += actual;

out:
	mkiss_put(ax);
}