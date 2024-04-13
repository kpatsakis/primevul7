void bpf_prog_destroy(struct bpf_prog *fp)
{
	__bpf_prog_release(fp);
}