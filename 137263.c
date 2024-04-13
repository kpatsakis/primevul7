bpf_get_prog_addr_region(const struct bpf_prog *prog,
			 unsigned long *symbol_start,
			 unsigned long *symbol_end)
{
	const struct bpf_binary_header *hdr = bpf_jit_binary_hdr(prog);
	unsigned long addr = (unsigned long)hdr;

	WARN_ON_ONCE(!bpf_prog_ebpf_jited(prog));

	*symbol_start = addr;
	*symbol_end   = addr + hdr->pages * PAGE_SIZE;
}