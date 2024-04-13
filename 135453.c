int snd_rawmidi_drain_output(struct snd_rawmidi_substream *substream)
{
	int err;
	long timeout;
	struct snd_rawmidi_runtime *runtime = substream->runtime;

	err = 0;
	runtime->drain = 1;
	timeout = wait_event_interruptible_timeout(runtime->sleep,
				(runtime->avail >= runtime->buffer_size),
				10*HZ);
	if (signal_pending(current))
		err = -ERESTARTSYS;
	if (runtime->avail < runtime->buffer_size && !timeout) {
		rmidi_warn(substream->rmidi,
			   "rawmidi drain error (avail = %li, buffer_size = %li)\n",
			   (long)runtime->avail, (long)runtime->buffer_size);
		err = -EIO;
	}
	runtime->drain = 0;
	if (err != -ERESTARTSYS) {
		/* we need wait a while to make sure that Tx FIFOs are empty */
		if (substream->ops->drain)
			substream->ops->drain(substream);
		else
			msleep(50);
		snd_rawmidi_drop_output(substream);
	}
	return err;
}