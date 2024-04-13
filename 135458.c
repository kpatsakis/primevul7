static int snd_rawmidi_free(struct snd_rawmidi *rmidi)
{
	if (!rmidi)
		return 0;

	snd_info_free_entry(rmidi->proc_entry);
	rmidi->proc_entry = NULL;
	mutex_lock(&register_mutex);
	if (rmidi->ops && rmidi->ops->dev_unregister)
		rmidi->ops->dev_unregister(rmidi);
	mutex_unlock(&register_mutex);

	snd_rawmidi_free_substreams(&rmidi->streams[SNDRV_RAWMIDI_STREAM_INPUT]);
	snd_rawmidi_free_substreams(&rmidi->streams[SNDRV_RAWMIDI_STREAM_OUTPUT]);
	if (rmidi->private_free)
		rmidi->private_free(rmidi);
	put_device(&rmidi->dev);
	return 0;
}