static inline int snd_rawmidi_ready_append(struct snd_rawmidi_substream *substream,
					   size_t count)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;
	return runtime->avail >= runtime->avail_min &&
	       (!substream->append || runtime->avail >= count);
}