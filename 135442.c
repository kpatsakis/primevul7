static void snd_rawmidi_input_event_work(struct work_struct *work)
{
	struct snd_rawmidi_runtime *runtime =
		container_of(work, struct snd_rawmidi_runtime, event_work);
	if (runtime->event)
		runtime->event(runtime->substream);
}