static int snd_rawmidi_dev_free(struct snd_device *device)
{
	struct snd_rawmidi *rmidi = device->device_data;
	return snd_rawmidi_free(rmidi);
}