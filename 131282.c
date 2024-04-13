static int check_tty(struct tty_struct *tty)
{
	if (!tty) {
		pr_warn("%s: I/O error, deactivating speakup\n",
			spk_ttyio_synth->long_name);
		/* No synth any more, so nobody will restart TTYs, and we thus
		 * need to do it ourselves.  Now that there is no synth we can
		 * let application flood anyway
		 */
		spk_ttyio_synth->alive = 0;
		speakup_start_ttys();
		return 1;
	}

	return 0;
}