int bpf_get_kallsym(unsigned int symnum, unsigned long *value, char *type,
		    char *sym)
{
	unsigned long symbol_start, symbol_end;
	struct bpf_prog_aux *aux;
	unsigned int it = 0;
	int ret = -ERANGE;

	if (!bpf_jit_kallsyms_enabled())
		return ret;

	rcu_read_lock();
	list_for_each_entry_rcu(aux, &bpf_kallsyms, ksym_lnode) {
		if (it++ != symnum)
			continue;

		bpf_get_prog_addr_region(aux->prog, &symbol_start, &symbol_end);
		bpf_get_prog_name(aux->prog, sym);

		*value = symbol_start;
		*type  = BPF_SYM_ELF_TYPE;

		ret = 0;
		break;
	}
	rcu_read_unlock();

	return ret;
}