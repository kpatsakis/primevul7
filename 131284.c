static int spk_ttyio_out_unicode(struct spk_synth *in_synth, u16 ch)
{
	int ret;

	if (ch < 0x80) {
		ret = spk_ttyio_out(in_synth, ch);
	} else if (ch < 0x800) {
		ret  = spk_ttyio_out(in_synth, 0xc0 | (ch >> 6));
		ret &= spk_ttyio_out(in_synth, 0x80 | (ch & 0x3f));
	} else {
		ret  = spk_ttyio_out(in_synth, 0xe0 | (ch >> 12));
		ret &= spk_ttyio_out(in_synth, 0x80 | ((ch >> 6) & 0x3f));
		ret &= spk_ttyio_out(in_synth, 0x80 | (ch & 0x3f));
	}
	return ret;
}