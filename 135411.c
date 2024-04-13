static int __init alsa_rawmidi_init(void)
{

	snd_ctl_register_ioctl(snd_rawmidi_control_ioctl);
	snd_ctl_register_ioctl_compat(snd_rawmidi_control_ioctl);
#ifdef CONFIG_SND_OSSEMUL
	{ int i;
	/* check device map table */
	for (i = 0; i < SNDRV_CARDS; i++) {
		if (midi_map[i] < 0 || midi_map[i] >= SNDRV_RAWMIDI_DEVICES) {
			pr_err("ALSA: rawmidi: invalid midi_map[%d] = %d\n",
			       i, midi_map[i]);
			midi_map[i] = 0;
		}
		if (amidi_map[i] < 0 || amidi_map[i] >= SNDRV_RAWMIDI_DEVICES) {
			pr_err("ALSA: rawmidi: invalid amidi_map[%d] = %d\n",
			       i, amidi_map[i]);
			amidi_map[i] = 1;
		}
	}
	}
#endif /* CONFIG_SND_OSSEMUL */
	return 0;
}