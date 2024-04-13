static int snd_rawmidi_open(struct inode *inode, struct file *file)
{
	int maj = imajor(inode);
	struct snd_card *card;
	int subdevice;
	unsigned short fflags;
	int err;
	struct snd_rawmidi *rmidi;
	struct snd_rawmidi_file *rawmidi_file = NULL;
	wait_queue_entry_t wait;

	if ((file->f_flags & O_APPEND) && !(file->f_flags & O_NONBLOCK)) 
		return -EINVAL;		/* invalid combination */

	err = nonseekable_open(inode, file);
	if (err < 0)
		return err;

	if (maj == snd_major) {
		rmidi = snd_lookup_minor_data(iminor(inode),
					      SNDRV_DEVICE_TYPE_RAWMIDI);
#ifdef CONFIG_SND_OSSEMUL
	} else if (maj == SOUND_MAJOR) {
		rmidi = snd_lookup_oss_minor_data(iminor(inode),
						  SNDRV_OSS_DEVICE_TYPE_MIDI);
#endif
	} else
		return -ENXIO;

	if (rmidi == NULL)
		return -ENODEV;

	if (!try_module_get(rmidi->card->module)) {
		snd_card_unref(rmidi->card);
		return -ENXIO;
	}

	mutex_lock(&rmidi->open_mutex);
	card = rmidi->card;
	err = snd_card_file_add(card, file);
	if (err < 0)
		goto __error_card;
	fflags = snd_rawmidi_file_flags(file);
	if ((file->f_flags & O_APPEND) || maj == SOUND_MAJOR) /* OSS emul? */
		fflags |= SNDRV_RAWMIDI_LFLG_APPEND;
	rawmidi_file = kmalloc(sizeof(*rawmidi_file), GFP_KERNEL);
	if (rawmidi_file == NULL) {
		err = -ENOMEM;
		goto __error;
	}
	init_waitqueue_entry(&wait, current);
	add_wait_queue(&rmidi->open_wait, &wait);
	while (1) {
		subdevice = snd_ctl_get_preferred_subdevice(card, SND_CTL_SUBDEV_RAWMIDI);
		err = rawmidi_open_priv(rmidi, subdevice, fflags, rawmidi_file);
		if (err >= 0)
			break;
		if (err == -EAGAIN) {
			if (file->f_flags & O_NONBLOCK) {
				err = -EBUSY;
				break;
			}
		} else
			break;
		set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&rmidi->open_mutex);
		schedule();
		mutex_lock(&rmidi->open_mutex);
		if (rmidi->card->shutdown) {
			err = -ENODEV;
			break;
		}
		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			break;
		}
	}
	remove_wait_queue(&rmidi->open_wait, &wait);
	if (err < 0) {
		kfree(rawmidi_file);
		goto __error;
	}
#ifdef CONFIG_SND_OSSEMUL
	if (rawmidi_file->input && rawmidi_file->input->runtime)
		rawmidi_file->input->runtime->oss = (maj == SOUND_MAJOR);
	if (rawmidi_file->output && rawmidi_file->output->runtime)
		rawmidi_file->output->runtime->oss = (maj == SOUND_MAJOR);
#endif
	file->private_data = rawmidi_file;
	mutex_unlock(&rmidi->open_mutex);
	snd_card_unref(rmidi->card);
	return 0;

 __error:
	snd_card_file_remove(card, file);
 __error_card:
	mutex_unlock(&rmidi->open_mutex);
	module_put(rmidi->card->module);
	snd_card_unref(rmidi->card);
	return err;
}