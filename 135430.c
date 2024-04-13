int snd_rawmidi_kernel_open(struct snd_card *card, int device, int subdevice,
			    int mode, struct snd_rawmidi_file * rfile)
{
	struct snd_rawmidi *rmidi;
	int err;

	if (snd_BUG_ON(!rfile))
		return -EINVAL;

	mutex_lock(&register_mutex);
	rmidi = snd_rawmidi_search(card, device);
	if (rmidi == NULL) {
		mutex_unlock(&register_mutex);
		return -ENODEV;
	}
	if (!try_module_get(rmidi->card->module)) {
		mutex_unlock(&register_mutex);
		return -ENXIO;
	}
	mutex_unlock(&register_mutex);

	mutex_lock(&rmidi->open_mutex);
	err = rawmidi_open_priv(rmidi, subdevice, mode, rfile);
	mutex_unlock(&rmidi->open_mutex);
	if (err < 0)
		module_put(rmidi->card->module);
	return err;
}