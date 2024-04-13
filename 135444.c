static void snd_rawmidi_free_substreams(struct snd_rawmidi_str *stream)
{
	struct snd_rawmidi_substream *substream;

	while (!list_empty(&stream->substreams)) {
		substream = list_entry(stream->substreams.next, struct snd_rawmidi_substream, list);
		list_del(&substream->list);
		kfree(substream);
	}
}