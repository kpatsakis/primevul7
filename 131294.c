static unsigned char ttyio_in(int timeout)
{
	struct spk_ldisc_data *ldisc_data = speakup_tty->disc_data;
	char rv;

	if (wait_for_completion_timeout(&ldisc_data->completion,
					usecs_to_jiffies(timeout)) == 0) {
		if (timeout)
			pr_warn("spk_ttyio: timeout (%d)  while waiting for input\n",
				timeout);
		return 0xff;
	}

	rv = ldisc_data->buf;
	/* Make sure we have read buf before we set buf_free to let
	 * the producer overwrite it
	 */
	mb();
	ldisc_data->buf_free = true;
	/* Let TTY push more characters */
	tty_schedule_flip(speakup_tty->port);

	return rv;
}