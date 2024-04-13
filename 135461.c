static int assign_substream(struct snd_rawmidi *rmidi, int subdevice,
			    int stream, int mode,
			    struct snd_rawmidi_substream **sub_ret)
{
	struct snd_rawmidi_substream *substream;
	struct snd_rawmidi_str *s = &rmidi->streams[stream];
	static unsigned int info_flags[2] = {
		[SNDRV_RAWMIDI_STREAM_OUTPUT] = SNDRV_RAWMIDI_INFO_OUTPUT,
		[SNDRV_RAWMIDI_STREAM_INPUT] = SNDRV_RAWMIDI_INFO_INPUT,
	};

	if (!(rmidi->info_flags & info_flags[stream]))
		return -ENXIO;
	if (subdevice >= 0 && subdevice >= s->substream_count)
		return -ENODEV;

	list_for_each_entry(substream, &s->substreams, list) {
		if (substream->opened) {
			if (stream == SNDRV_RAWMIDI_STREAM_INPUT ||
			    !(mode & SNDRV_RAWMIDI_LFLG_APPEND) ||
			    !substream->append)
				continue;
		}
		if (subdevice < 0 || subdevice == substream->number) {
			*sub_ret = substream;
			return 0;
		}
	}
	return -EAGAIN;
}