int snd_rawmidi_drain_input(struct snd_rawmidi_substream *substream)
{
	unsigned long flags;
	struct snd_rawmidi_runtime *runtime = substream->runtime;

	snd_rawmidi_input_trigger(substream, 0);
	runtime->drain = 0;
	spin_lock_irqsave(&runtime->lock, flags);
	runtime->appl_ptr = runtime->hw_ptr = 0;
	runtime->avail = 0;
	spin_unlock_irqrestore(&runtime->lock, flags);
	return 0;
}