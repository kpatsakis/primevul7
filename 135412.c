static void snd_rawmidi_proc_info_read(struct snd_info_entry *entry,
				       struct snd_info_buffer *buffer)
{
	struct snd_rawmidi *rmidi;
	struct snd_rawmidi_substream *substream;
	struct snd_rawmidi_runtime *runtime;

	rmidi = entry->private_data;
	snd_iprintf(buffer, "%s\n\n", rmidi->name);
	mutex_lock(&rmidi->open_mutex);
	if (rmidi->info_flags & SNDRV_RAWMIDI_INFO_OUTPUT) {
		list_for_each_entry(substream,
				    &rmidi->streams[SNDRV_RAWMIDI_STREAM_OUTPUT].substreams,
				    list) {
			snd_iprintf(buffer,
				    "Output %d\n"
				    "  Tx bytes     : %lu\n",
				    substream->number,
				    (unsigned long) substream->bytes);
			if (substream->opened) {
				snd_iprintf(buffer,
				    "  Owner PID    : %d\n",
				    pid_vnr(substream->pid));
				runtime = substream->runtime;
				snd_iprintf(buffer,
				    "  Mode         : %s\n"
				    "  Buffer size  : %lu\n"
				    "  Avail        : %lu\n",
				    runtime->oss ? "OSS compatible" : "native",
				    (unsigned long) runtime->buffer_size,
				    (unsigned long) runtime->avail);
			}
		}
	}
	if (rmidi->info_flags & SNDRV_RAWMIDI_INFO_INPUT) {
		list_for_each_entry(substream,
				    &rmidi->streams[SNDRV_RAWMIDI_STREAM_INPUT].substreams,
				    list) {
			snd_iprintf(buffer,
				    "Input %d\n"
				    "  Rx bytes     : %lu\n",
				    substream->number,
				    (unsigned long) substream->bytes);
			if (substream->opened) {
				snd_iprintf(buffer,
					    "  Owner PID    : %d\n",
					    pid_vnr(substream->pid));
				runtime = substream->runtime;
				snd_iprintf(buffer,
					    "  Buffer size  : %lu\n"
					    "  Avail        : %lu\n"
					    "  Overruns     : %lu\n",
					    (unsigned long) runtime->buffer_size,
					    (unsigned long) runtime->avail,
					    (unsigned long) runtime->xruns);
			}
		}
	}
	mutex_unlock(&rmidi->open_mutex);
}