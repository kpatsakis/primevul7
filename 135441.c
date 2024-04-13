static struct snd_rawmidi *snd_rawmidi_search(struct snd_card *card, int device)
{
	struct snd_rawmidi *rawmidi;

	list_for_each_entry(rawmidi, &snd_rawmidi_devices, list)
		if (rawmidi->card == card && rawmidi->device == device)
			return rawmidi;
	return NULL;
}