static void rawmidi_release_priv(struct snd_rawmidi_file *rfile)
{
	struct snd_rawmidi *rmidi;

	rmidi = rfile->rmidi;
	mutex_lock(&rmidi->open_mutex);
	if (rfile->input) {
		close_substream(rmidi, rfile->input, 1);
		rfile->input = NULL;
	}
	if (rfile->output) {
		close_substream(rmidi, rfile->output, 1);
		rfile->output = NULL;
	}
	rfile->rmidi = NULL;
	mutex_unlock(&rmidi->open_mutex);
	wake_up(&rmidi->open_wait);
}