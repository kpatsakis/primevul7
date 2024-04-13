void bpf_user_rnd_init_once(void)
{
	prandom_init_once(&bpf_user_rnd_state);
}