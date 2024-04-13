static void release_rawmidi_device(struct device *dev)
{
	kfree(container_of(dev, struct snd_rawmidi, dev));
}