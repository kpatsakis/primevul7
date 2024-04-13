static int snd_rawmidi_control_ioctl(struct snd_card *card,
				     struct snd_ctl_file *control,
				     unsigned int cmd,
				     unsigned long arg)
{
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case SNDRV_CTL_IOCTL_RAWMIDI_NEXT_DEVICE:
	{
		int device;
		
		if (get_user(device, (int __user *)argp))
			return -EFAULT;
		if (device >= SNDRV_RAWMIDI_DEVICES) /* next device is -1 */
			device = SNDRV_RAWMIDI_DEVICES - 1;
		mutex_lock(&register_mutex);
		device = device < 0 ? 0 : device + 1;
		while (device < SNDRV_RAWMIDI_DEVICES) {
			if (snd_rawmidi_search(card, device))
				break;
			device++;
		}
		if (device == SNDRV_RAWMIDI_DEVICES)
			device = -1;
		mutex_unlock(&register_mutex);
		if (put_user(device, (int __user *)argp))
			return -EFAULT;
		return 0;
	}
	case SNDRV_CTL_IOCTL_RAWMIDI_PREFER_SUBDEVICE:
	{
		int val;
		
		if (get_user(val, (int __user *)argp))
			return -EFAULT;
		control->preferred_subdevice[SND_CTL_SUBDEV_RAWMIDI] = val;
		return 0;
	}
	case SNDRV_CTL_IOCTL_RAWMIDI_INFO:
		return snd_rawmidi_info_select_user(card, argp);
	}
	return -ENOIOCTLCMD;
}