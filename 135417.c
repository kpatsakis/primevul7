static int snd_rawmidi_info_select_user(struct snd_card *card,
					struct snd_rawmidi_info __user *_info)
{
	int err;
	struct snd_rawmidi_info info;
	if (get_user(info.device, &_info->device))
		return -EFAULT;
	if (get_user(info.stream, &_info->stream))
		return -EFAULT;
	if (get_user(info.subdevice, &_info->subdevice))
		return -EFAULT;
	if ((err = snd_rawmidi_info_select(card, &info)) < 0)
		return err;
	if (copy_to_user(_info, &info, sizeof(struct snd_rawmidi_info)))
		return -EFAULT;
	return 0;
}