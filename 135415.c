static long snd_rawmidi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct snd_rawmidi_file *rfile;
	void __user *argp = (void __user *)arg;

	rfile = file->private_data;
	if (((cmd >> 8) & 0xff) != 'W')
		return -ENOTTY;
	switch (cmd) {
	case SNDRV_RAWMIDI_IOCTL_PVERSION:
		return put_user(SNDRV_RAWMIDI_VERSION, (int __user *)argp) ? -EFAULT : 0;
	case SNDRV_RAWMIDI_IOCTL_INFO:
	{
		int stream;
		struct snd_rawmidi_info __user *info = argp;
		if (get_user(stream, &info->stream))
			return -EFAULT;
		switch (stream) {
		case SNDRV_RAWMIDI_STREAM_INPUT:
			return snd_rawmidi_info_user(rfile->input, info);
		case SNDRV_RAWMIDI_STREAM_OUTPUT:
			return snd_rawmidi_info_user(rfile->output, info);
		default:
			return -EINVAL;
		}
	}
	case SNDRV_RAWMIDI_IOCTL_PARAMS:
	{
		struct snd_rawmidi_params params;
		if (copy_from_user(&params, argp, sizeof(struct snd_rawmidi_params)))
			return -EFAULT;
		switch (params.stream) {
		case SNDRV_RAWMIDI_STREAM_OUTPUT:
			if (rfile->output == NULL)
				return -EINVAL;
			return snd_rawmidi_output_params(rfile->output, &params);
		case SNDRV_RAWMIDI_STREAM_INPUT:
			if (rfile->input == NULL)
				return -EINVAL;
			return snd_rawmidi_input_params(rfile->input, &params);
		default:
			return -EINVAL;
		}
	}
	case SNDRV_RAWMIDI_IOCTL_STATUS:
	{
		int err = 0;
		struct snd_rawmidi_status status;
		if (copy_from_user(&status, argp, sizeof(struct snd_rawmidi_status)))
			return -EFAULT;
		switch (status.stream) {
		case SNDRV_RAWMIDI_STREAM_OUTPUT:
			if (rfile->output == NULL)
				return -EINVAL;
			err = snd_rawmidi_output_status(rfile->output, &status);
			break;
		case SNDRV_RAWMIDI_STREAM_INPUT:
			if (rfile->input == NULL)
				return -EINVAL;
			err = snd_rawmidi_input_status(rfile->input, &status);
			break;
		default:
			return -EINVAL;
		}
		if (err < 0)
			return err;
		if (copy_to_user(argp, &status, sizeof(struct snd_rawmidi_status)))
			return -EFAULT;
		return 0;
	}
	case SNDRV_RAWMIDI_IOCTL_DROP:
	{
		int val;
		if (get_user(val, (int __user *) argp))
			return -EFAULT;
		switch (val) {
		case SNDRV_RAWMIDI_STREAM_OUTPUT:
			if (rfile->output == NULL)
				return -EINVAL;
			return snd_rawmidi_drop_output(rfile->output);
		default:
			return -EINVAL;
		}
	}
	case SNDRV_RAWMIDI_IOCTL_DRAIN:
	{
		int val;
		if (get_user(val, (int __user *) argp))
			return -EFAULT;
		switch (val) {
		case SNDRV_RAWMIDI_STREAM_OUTPUT:
			if (rfile->output == NULL)
				return -EINVAL;
			return snd_rawmidi_drain_output(rfile->output);
		case SNDRV_RAWMIDI_STREAM_INPUT:
			if (rfile->input == NULL)
				return -EINVAL;
			return snd_rawmidi_drain_input(rfile->input);
		default:
			return -EINVAL;
		}
	}
	default:
		rmidi_dbg(rfile->rmidi,
			  "rawmidi: unknown command = 0x%x\n", cmd);
	}
	return -ENOTTY;
}