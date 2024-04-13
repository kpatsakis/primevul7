void __bpf_prog_free(struct bpf_prog *fp)
{
	kfree(fp->aux);
	vfree(fp);
}