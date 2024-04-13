static void spk_ttyio_flush_buffer(void)
{
	mutex_lock(&speakup_tty_mutex);
	if (check_tty(speakup_tty)) {
		mutex_unlock(&speakup_tty_mutex);
		return;
	}

	if (speakup_tty->ops->flush_buffer)
		speakup_tty->ops->flush_buffer(speakup_tty);

	mutex_unlock(&speakup_tty_mutex);
}