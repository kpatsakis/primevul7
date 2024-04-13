bool is_bpf_text_address(unsigned long addr)
{
	bool ret;

	rcu_read_lock();
	ret = bpf_prog_kallsyms_find(addr) != NULL;
	rcu_read_unlock();

	return ret;
}