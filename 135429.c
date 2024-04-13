static int snd_rawmidi_info_user(struct snd_rawmidi_substream *substream,
				 struct snd_rawmidi_info __user * _info)
{
	struct snd_rawmidi_info info;
	int err;
	if ((err = snd_rawmidi_info(substream, &info)) < 0)
		return err;
	if (copy_to_user(_info, &info, sizeof(struct snd_rawmidi_info)))
		return -EFAULT;
	return 0;
}