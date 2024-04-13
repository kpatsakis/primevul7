static void spk_ttyio_tiocmset(unsigned int set, unsigned int clear)
{
	mutex_lock(&speakup_tty_mutex);
	if (check_tty(speakup_tty)) {
		mutex_unlock(&speakup_tty_mutex);
		return;
	}

	if (speakup_tty->ops->tiocmset)
		speakup_tty->ops->tiocmset(speakup_tty, set, clear);
	mutex_unlock(&speakup_tty_mutex);
}