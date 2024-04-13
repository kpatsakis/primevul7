static inline void snd_rawmidi_output_trigger(struct snd_rawmidi_substream *substream,int up)
{
	if (!substream->opened)
		return;
	substream->ops->trigger(substream, up);
}