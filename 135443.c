int snd_rawmidi_transmit_ack(struct snd_rawmidi_substream *substream, int count)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;
	int result;
	unsigned long flags;

	spin_lock_irqsave(&runtime->lock, flags);
	result = __snd_rawmidi_transmit_ack(substream, count);
	spin_unlock_irqrestore(&runtime->lock, flags);
	return result;
}