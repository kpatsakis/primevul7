static bool bpf_prog_kallsyms_verify_off(const struct bpf_prog *fp)
{
	return list_empty(&fp->aux->ksym_lnode) ||
	       fp->aux->ksym_lnode.prev == LIST_POISON2;
}