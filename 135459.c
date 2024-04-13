static int snd_rawmidi_output_status(struct snd_rawmidi_substream *substream,
				     struct snd_rawmidi_status * status)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;

	memset(status, 0, sizeof(*status));
	status->stream = SNDRV_RAWMIDI_STREAM_OUTPUT;
	spin_lock_irq(&runtime->lock);
	status->avail = runtime->avail;
	spin_unlock_irq(&runtime->lock);
	return 0;
}