static void spk_ttyio_ldisc_close(struct tty_struct *tty)
{
	mutex_lock(&speakup_tty_mutex);
	kfree(speakup_tty->disc_data);
	speakup_tty = NULL;
	mutex_unlock(&speakup_tty_mutex);
}