static int __snd_rawmidi_info_select(struct snd_card *card,
				     struct snd_rawmidi_info *info)
{
	struct snd_rawmidi *rmidi;
	struct snd_rawmidi_str *pstr;
	struct snd_rawmidi_substream *substream;

	rmidi = snd_rawmidi_search(card, info->device);
	if (!rmidi)
		return -ENXIO;
	if (info->stream < 0 || info->stream > 1)
		return -EINVAL;
	pstr = &rmidi->streams[info->stream];
	if (pstr->substream_count == 0)
		return -ENOENT;
	if (info->subdevice >= pstr->substream_count)
		return -ENXIO;
	list_for_each_entry(substream, &pstr->substreams, list) {
		if ((unsigned int)substream->number == info->subdevice)
			return snd_rawmidi_info(substream, info);
	}
	return -ENXIO;
}