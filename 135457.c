static int snd_rawmidi_dev_disconnect(struct snd_device *device)
{
	struct snd_rawmidi *rmidi = device->device_data;
	int dir;

	mutex_lock(&register_mutex);
	mutex_lock(&rmidi->open_mutex);
	wake_up(&rmidi->open_wait);
	list_del_init(&rmidi->list);
	for (dir = 0; dir < 2; dir++) {
		struct snd_rawmidi_substream *s;
		list_for_each_entry(s, &rmidi->streams[dir].substreams, list) {
			if (s->runtime)
				wake_up(&s->runtime->sleep);
		}
	}

#ifdef CONFIG_SND_OSSEMUL
	if (rmidi->ossreg) {
		if ((int)rmidi->device == midi_map[rmidi->card->number]) {
			snd_unregister_oss_device(SNDRV_OSS_DEVICE_TYPE_MIDI, rmidi->card, 0);
#ifdef SNDRV_OSS_INFO_DEV_MIDI
			snd_oss_info_unregister(SNDRV_OSS_INFO_DEV_MIDI, rmidi->card->number);
#endif
		}
		if ((int)rmidi->device == amidi_map[rmidi->card->number])
			snd_unregister_oss_device(SNDRV_OSS_DEVICE_TYPE_MIDI, rmidi->card, 1);
		rmidi->ossreg = 0;
	}
#endif /* CONFIG_SND_OSSEMUL */
	snd_unregister_device(&rmidi->dev);
	mutex_unlock(&rmidi->open_mutex);
	mutex_unlock(&register_mutex);
	return 0;
}