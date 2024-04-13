int snd_rawmidi_kernel_release(struct snd_rawmidi_file *rfile)
{
	struct snd_rawmidi *rmidi;

	if (snd_BUG_ON(!rfile))
		return -ENXIO;
	
	rmidi = rfile->rmidi;
	rawmidi_release_priv(rfile);
	module_put(rmidi->card->module);
	return 0;
}