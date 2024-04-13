static int snd_rawmidi_input_status(struct snd_rawmidi_substream *substream,
				    struct snd_rawmidi_status * status)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;

	memset(status, 0, sizeof(*status));
	status->stream = SNDRV_RAWMIDI_STREAM_INPUT;
	spin_lock_irq(&runtime->lock);
	status->avail = runtime->avail;
	status->xruns = runtime->xruns;
	runtime->xruns = 0;
	spin_unlock_irq(&runtime->lock);
	return 0;
}