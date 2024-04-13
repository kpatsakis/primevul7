void spk_ttyio_unregister_ldisc(void)
{
	if (tty_unregister_ldisc(N_SPEAKUP))
		pr_warn("speakup: Couldn't unregister ldisc\n");
}