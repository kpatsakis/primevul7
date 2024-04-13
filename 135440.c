static int snd_rawmidi_runtime_create(struct snd_rawmidi_substream *substream)
{
	struct snd_rawmidi_runtime *runtime;

	if ((runtime = kzalloc(sizeof(*runtime), GFP_KERNEL)) == NULL)
		return -ENOMEM;
	runtime->substream = substream;
	spin_lock_init(&runtime->lock);
	init_waitqueue_head(&runtime->sleep);
	INIT_WORK(&runtime->event_work, snd_rawmidi_input_event_work);
	runtime->event = NULL;
	runtime->buffer_size = PAGE_SIZE;
	runtime->avail_min = 1;
	if (substream->stream == SNDRV_RAWMIDI_STREAM_INPUT)
		runtime->avail = 0;
	else
		runtime->avail = runtime->buffer_size;
	if ((runtime->buffer = kmalloc(runtime->buffer_size, GFP_KERNEL)) == NULL) {
		kfree(runtime);
		return -ENOMEM;
	}
	runtime->appl_ptr = runtime->hw_ptr = 0;
	substream->runtime = runtime;
	return 0;
}