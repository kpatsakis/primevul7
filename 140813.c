static int ct_seq_show(struct seq_file *s, void *v)
{
	struct nf_conntrack_tuple_hash *hash = v;
	struct nf_conn *ct = nf_ct_tuplehash_to_ctrack(hash);
	const struct nf_conntrack_l4proto *l4proto;
	struct net *net = seq_file_net(s);
	int ret = 0;

	WARN_ON(!ct);
	if (unlikely(!atomic_inc_not_zero(&ct->ct_general.use)))
		return 0;

	if (nf_ct_should_gc(ct)) {
		nf_ct_kill(ct);
		goto release;
	}

	/* we only want to print DIR_ORIGINAL */
	if (NF_CT_DIRECTION(hash))
		goto release;

	if (!net_eq(nf_ct_net(ct), net))
		goto release;

	l4proto = nf_ct_l4proto_find(nf_ct_protonum(ct));

	ret = -ENOSPC;
	seq_printf(s, "%-8s %u %-8s %u ",
		   l3proto_name(nf_ct_l3num(ct)), nf_ct_l3num(ct),
		   l4proto_name(l4proto->l4proto), nf_ct_protonum(ct));

	if (!test_bit(IPS_OFFLOAD_BIT, &ct->status))
		seq_printf(s, "%ld ", nf_ct_expires(ct)  / HZ);

	if (l4proto->print_conntrack)
		l4proto->print_conntrack(s, ct);

	print_tuple(s, &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple,
		    l4proto);

	ct_show_zone(s, ct, NF_CT_ZONE_DIR_ORIG);

	if (seq_has_overflowed(s))
		goto release;

	if (seq_print_acct(s, ct, IP_CT_DIR_ORIGINAL))
		goto release;

	if (!(test_bit(IPS_SEEN_REPLY_BIT, &ct->status)))
		seq_puts(s, "[UNREPLIED] ");

	print_tuple(s, &ct->tuplehash[IP_CT_DIR_REPLY].tuple, l4proto);

	ct_show_zone(s, ct, NF_CT_ZONE_DIR_REPL);

	if (seq_print_acct(s, ct, IP_CT_DIR_REPLY))
		goto release;

	if (test_bit(IPS_HW_OFFLOAD_BIT, &ct->status))
		seq_puts(s, "[HW_OFFLOAD] ");
	else if (test_bit(IPS_OFFLOAD_BIT, &ct->status))
		seq_puts(s, "[OFFLOAD] ");
	else if (test_bit(IPS_ASSURED_BIT, &ct->status))
		seq_puts(s, "[ASSURED] ");

	if (seq_has_overflowed(s))
		goto release;

#if defined(CONFIG_NF_CONNTRACK_MARK)
	seq_printf(s, "mark=%u ", ct->mark);
#endif

	ct_show_secctx(s, ct);
	ct_show_zone(s, ct, NF_CT_DEFAULT_ZONE_DIR);
	ct_show_delta_time(s, ct);

	seq_printf(s, "use=%u\n", atomic_read(&ct->ct_general.use));

	if (seq_has_overflowed(s))
		goto release;

	ret = 0;
release:
	nf_ct_put(ct);
	return ret;
}