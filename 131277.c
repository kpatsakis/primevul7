static int spk_ttyio_out(struct spk_synth *in_synth, const char ch)
{
	mutex_lock(&speakup_tty_mutex);
	if (in_synth->alive && speakup_tty && speakup_tty->ops->write) {
		int ret = speakup_tty->ops->write(speakup_tty, &ch, 1);

		mutex_unlock(&speakup_tty_mutex);
		if (ret == 0)
			/* No room */
			return 0;
		if (ret < 0) {
			pr_warn("%s: I/O error, deactivating speakup\n",
				in_synth->long_name);
			/* No synth any more, so nobody will restart TTYs,
			 * and we thus need to do it ourselves.  Now that there
			 * is no synth we can let application flood anyway
			 */
			in_synth->alive = 0;
			speakup_start_ttys();
			return 0;
		}
		return 1;
	}

	mutex_unlock(&speakup_tty_mutex);
	return 0;
}