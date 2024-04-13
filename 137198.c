void bpf_jit_binary_free(struct bpf_binary_header *hdr)
{
	module_memfree(hdr);
}