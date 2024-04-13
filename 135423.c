int snd_rawmidi_info_select(struct snd_card *card, struct snd_rawmidi_info *info)
{
	int ret;

	mutex_lock(&register_mutex);
	ret = __snd_rawmidi_info_select(card, info);
	mutex_unlock(&register_mutex);
	return ret;
}