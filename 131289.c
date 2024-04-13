static void spk_ttyio_send_xchar(char ch)
{
	mutex_lock(&speakup_tty_mutex);
	if (check_tty(speakup_tty)) {
		mutex_unlock(&speakup_tty_mutex);
		return;
	}

	if (speakup_tty->ops->send_xchar)
		speakup_tty->ops->send_xchar(speakup_tty, ch);
	mutex_unlock(&speakup_tty_mutex);
}