static int snd_rawmidi_dev_register(struct snd_device *device)
{
	int err;
	struct snd_info_entry *entry;
	char name[16];
	struct snd_rawmidi *rmidi = device->device_data;

	if (rmidi->device >= SNDRV_RAWMIDI_DEVICES)
		return -ENOMEM;
	mutex_lock(&register_mutex);
	if (snd_rawmidi_search(rmidi->card, rmidi->device)) {
		mutex_unlock(&register_mutex);
		return -EBUSY;
	}
	list_add_tail(&rmidi->list, &snd_rawmidi_devices);
	mutex_unlock(&register_mutex);
	err = snd_register_device(SNDRV_DEVICE_TYPE_RAWMIDI,
				  rmidi->card, rmidi->device,
				  &snd_rawmidi_f_ops, rmidi, &rmidi->dev);
	if (err < 0) {
		rmidi_err(rmidi, "unable to register\n");
		mutex_lock(&register_mutex);
		list_del(&rmidi->list);
		mutex_unlock(&register_mutex);
		return err;
	}
	if (rmidi->ops && rmidi->ops->dev_register &&
	    (err = rmidi->ops->dev_register(rmidi)) < 0) {
		snd_unregister_device(&rmidi->dev);
		mutex_lock(&register_mutex);
		list_del(&rmidi->list);
		mutex_unlock(&register_mutex);
		return err;
	}
#ifdef CONFIG_SND_OSSEMUL
	rmidi->ossreg = 0;
	if ((int)rmidi->device == midi_map[rmidi->card->number]) {
		if (snd_register_oss_device(SNDRV_OSS_DEVICE_TYPE_MIDI,
					    rmidi->card, 0, &snd_rawmidi_f_ops,
					    rmidi) < 0) {
			rmidi_err(rmidi,
				  "unable to register OSS rawmidi device %i:%i\n",
				  rmidi->card->number, 0);
		} else {
			rmidi->ossreg++;
#ifdef SNDRV_OSS_INFO_DEV_MIDI
			snd_oss_info_register(SNDRV_OSS_INFO_DEV_MIDI, rmidi->card->number, rmidi->name);
#endif
		}
	}
	if ((int)rmidi->device == amidi_map[rmidi->card->number]) {
		if (snd_register_oss_device(SNDRV_OSS_DEVICE_TYPE_MIDI,
					    rmidi->card, 1, &snd_rawmidi_f_ops,
					    rmidi) < 0) {
			rmidi_err(rmidi,
				  "unable to register OSS rawmidi device %i:%i\n",
				  rmidi->card->number, 1);
		} else {
			rmidi->ossreg++;
		}
	}
#endif /* CONFIG_SND_OSSEMUL */
	sprintf(name, "midi%d", rmidi->device);
	entry = snd_info_create_card_entry(rmidi->card, name, rmidi->card->proc_root);
	if (entry) {
		entry->private_data = rmidi;
		entry->c.text.read = snd_rawmidi_proc_info_read;
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	rmidi->proc_entry = entry;
#if IS_ENABLED(CONFIG_SND_SEQUENCER)
	if (!rmidi->ops || !rmidi->ops->dev_register) { /* own registration mechanism */
		if (snd_seq_device_new(rmidi->card, rmidi->device, SNDRV_SEQ_DEV_ID_MIDISYNTH, 0, &rmidi->seq_dev) >= 0) {
			rmidi->seq_dev->private_data = rmidi;
			rmidi->seq_dev->private_free = snd_rawmidi_dev_seq_free;
			sprintf(rmidi->seq_dev->name, "MIDI %d-%d", rmidi->card->number, rmidi->device);
			snd_device_register(rmidi->card, rmidi->seq_dev);
		}
	}
#endif
	return 0;
}