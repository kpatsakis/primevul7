static int open_substream(struct snd_rawmidi *rmidi,
			  struct snd_rawmidi_substream *substream,
			  int mode)
{
	int err;

	if (substream->use_count == 0) {
		err = snd_rawmidi_runtime_create(substream);
		if (err < 0)
			return err;
		err = substream->ops->open(substream);
		if (err < 0) {
			snd_rawmidi_runtime_free(substream);
			return err;
		}
		substream->opened = 1;
		substream->active_sensing = 0;
		if (mode & SNDRV_RAWMIDI_LFLG_APPEND)
			substream->append = 1;
		substream->pid = get_pid(task_pid(current));
		rmidi->streams[substream->stream].substream_opened++;
	}
	substream->use_count++;
	return 0;
}