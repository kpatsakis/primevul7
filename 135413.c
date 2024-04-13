int snd_rawmidi_new(struct snd_card *card, char *id, int device,
		    int output_count, int input_count,
		    struct snd_rawmidi ** rrawmidi)
{
	struct snd_rawmidi *rmidi;
	int err;
	static struct snd_device_ops ops = {
		.dev_free = snd_rawmidi_dev_free,
		.dev_register = snd_rawmidi_dev_register,
		.dev_disconnect = snd_rawmidi_dev_disconnect,
	};

	if (snd_BUG_ON(!card))
		return -ENXIO;
	if (rrawmidi)
		*rrawmidi = NULL;
	rmidi = kzalloc(sizeof(*rmidi), GFP_KERNEL);
	if (!rmidi)
		return -ENOMEM;
	rmidi->card = card;
	rmidi->device = device;
	mutex_init(&rmidi->open_mutex);
	init_waitqueue_head(&rmidi->open_wait);
	INIT_LIST_HEAD(&rmidi->streams[SNDRV_RAWMIDI_STREAM_INPUT].substreams);
	INIT_LIST_HEAD(&rmidi->streams[SNDRV_RAWMIDI_STREAM_OUTPUT].substreams);

	if (id != NULL)
		strlcpy(rmidi->id, id, sizeof(rmidi->id));

	snd_device_initialize(&rmidi->dev, card);
	rmidi->dev.release = release_rawmidi_device;
	dev_set_name(&rmidi->dev, "midiC%iD%i", card->number, device);

	if ((err = snd_rawmidi_alloc_substreams(rmidi,
						&rmidi->streams[SNDRV_RAWMIDI_STREAM_INPUT],
						SNDRV_RAWMIDI_STREAM_INPUT,
						input_count)) < 0) {
		snd_rawmidi_free(rmidi);
		return err;
	}
	if ((err = snd_rawmidi_alloc_substreams(rmidi,
						&rmidi->streams[SNDRV_RAWMIDI_STREAM_OUTPUT],
						SNDRV_RAWMIDI_STREAM_OUTPUT,
						output_count)) < 0) {
		snd_rawmidi_free(rmidi);
		return err;
	}
	if ((err = snd_device_new(card, SNDRV_DEV_RAWMIDI, rmidi, &ops)) < 0) {
		snd_rawmidi_free(rmidi);
		return err;
	}
	if (rrawmidi)
		*rrawmidi = rmidi;
	return 0;
}