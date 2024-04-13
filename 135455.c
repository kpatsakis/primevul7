static void snd_rawmidi_input_trigger(struct snd_rawmidi_substream *substream, int up)
{
	if (!substream->opened)
		return;
	substream->ops->trigger(substream, up);
	if (!up)
		cancel_work_sync(&substream->runtime->event_work);
}