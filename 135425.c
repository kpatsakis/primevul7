int snd_rawmidi_transmit_peek(struct snd_rawmidi_substream *substream,
			      unsigned char *buffer, int count)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;
	int result;
	unsigned long flags;

	spin_lock_irqsave(&runtime->lock, flags);
	result = __snd_rawmidi_transmit_peek(substream, buffer, count);
	spin_unlock_irqrestore(&runtime->lock, flags);
	return result;
}