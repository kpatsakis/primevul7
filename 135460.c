static int rawmidi_open_priv(struct snd_rawmidi *rmidi, int subdevice, int mode,
			     struct snd_rawmidi_file *rfile)
{
	struct snd_rawmidi_substream *sinput = NULL, *soutput = NULL;
	int err;

	rfile->input = rfile->output = NULL;
	if (mode & SNDRV_RAWMIDI_LFLG_INPUT) {
		err = assign_substream(rmidi, subdevice,
				       SNDRV_RAWMIDI_STREAM_INPUT,
				       mode, &sinput);
		if (err < 0)
			return err;
	}
	if (mode & SNDRV_RAWMIDI_LFLG_OUTPUT) {
		err = assign_substream(rmidi, subdevice,
				       SNDRV_RAWMIDI_STREAM_OUTPUT,
				       mode, &soutput);
		if (err < 0)
			return err;
	}

	if (sinput) {
		err = open_substream(rmidi, sinput, mode);
		if (err < 0)
			return err;
	}
	if (soutput) {
		err = open_substream(rmidi, soutput, mode);
		if (err < 0) {
			if (sinput)
				close_substream(rmidi, sinput, 0);
			return err;
		}
	}

	rfile->rmidi = rmidi;
	rfile->input = sinput;
	rfile->output = soutput;
	return 0;
}