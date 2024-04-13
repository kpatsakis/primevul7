void spk_ttyio_release(void)
{
	if (!speakup_tty)
		return;

	tty_lock(speakup_tty);

	if (speakup_tty->ops->close)
		speakup_tty->ops->close(speakup_tty, NULL);

	tty_ldisc_flush(speakup_tty);
	tty_unlock(speakup_tty);
	tty_kclose(speakup_tty);
}