static void snd_rawmidi_dev_seq_free(struct snd_seq_device *device)
{
	struct snd_rawmidi *rmidi = device->private_data;
	rmidi->seq_dev = NULL;
}