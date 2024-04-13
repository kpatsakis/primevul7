int snd_rawmidi_transmit(struct snd_rawmidi_substream *substream,
			 unsigned char *buffer, int count)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;
	int result;
	unsigned long flags;

	spin_lock_irqsave(&runtime->lock, flags);
	if (!substream->opened)
		result = -EBADFD;
	else {
		count = __snd_rawmidi_transmit_peek(substream, buffer, count);
		if (count <= 0)
			result = count;
		else
			result = __snd_rawmidi_transmit_ack(substream, count);
	}
	spin_unlock_irqrestore(&runtime->lock, flags);
	return result;
}