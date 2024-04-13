void snd_rawmidi_set_ops(struct snd_rawmidi *rmidi, int stream,
			 const struct snd_rawmidi_ops *ops)
{
	struct snd_rawmidi_substream *substream;
	
	list_for_each_entry(substream, &rmidi->streams[stream].substreams, list)
		substream->ops = ops;
}