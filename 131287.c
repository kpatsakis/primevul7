const char *spk_ttyio_synth_immediate(struct spk_synth *synth, const char *buff)
{
	u_char ch;

	while ((ch = *buff)) {
		if (ch == '\n')
			ch = synth->procspeech;
		if (tty_write_room(speakup_tty) < 1 ||
		    !synth->io_ops->synth_out(synth, ch))
			return buff;
		buff++;
	}
	return NULL;
}