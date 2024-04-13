int spk_ttyio_synth_probe(struct spk_synth *synth)
{
	int rv = spk_ttyio_initialise_ldisc(synth);

	if (rv)
		return rv;

	synth->alive = 1;
	spk_ttyio_synth = synth;

	return 0;
}