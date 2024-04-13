const char *__bpf_address_lookup(unsigned long addr, unsigned long *size,
				 unsigned long *off, char *sym)
{
	unsigned long symbol_start, symbol_end;
	struct bpf_prog *prog;
	char *ret = NULL;

	rcu_read_lock();
	prog = bpf_prog_kallsyms_find(addr);
	if (prog) {
		bpf_get_prog_addr_region(prog, &symbol_start, &symbol_end);
		bpf_get_prog_name(prog, sym);

		ret = sym;
		if (size)
			*size = symbol_end - symbol_start;
		if (off)
			*off  = addr - symbol_start;
	}
	rcu_read_unlock();

	return ret;
}