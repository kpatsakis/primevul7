nf_conntrack_hash_sysctl(struct ctl_table *table, int write,
			 void *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	/* module_param hashsize could have changed value */
	nf_conntrack_htable_size_user = nf_conntrack_htable_size;

	ret = proc_dointvec(table, write, buffer, lenp, ppos);
	if (ret < 0 || !write)
		return ret;

	/* update ret, we might not be able to satisfy request */
	ret = nf_conntrack_hash_resize(nf_conntrack_htable_size_user);

	/* update it to the actual value used by conntrack */
	nf_conntrack_htable_size_user = nf_conntrack_htable_size;
	return ret;
}