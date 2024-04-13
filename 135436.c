static void __exit alsa_rawmidi_exit(void)
{
	snd_ctl_unregister_ioctl(snd_rawmidi_control_ioctl);
	snd_ctl_unregister_ioctl_compat(snd_rawmidi_control_ioctl);
}