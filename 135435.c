static int snd_rawmidi_runtime_free(struct snd_rawmidi_substream *substream)
{
	struct snd_rawmidi_runtime *runtime = substream->runtime;

	kfree(runtime->buffer);
	kfree(runtime);
	substream->runtime = NULL;
	return 0;
}