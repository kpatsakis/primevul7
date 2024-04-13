void spk_ttyio_register_ldisc(void)
{
	if (tty_register_ldisc(N_SPEAKUP, &spk_ttyio_ldisc_ops))
		pr_warn("speakup: Error registering line discipline. Most synths won't work.\n");
}