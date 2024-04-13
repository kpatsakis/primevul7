static void ct_show_zone(struct seq_file *s, const struct nf_conn *ct,
			 int dir)
{
	const struct nf_conntrack_zone *zone = nf_ct_zone(ct);

	if (zone->dir != dir)
		return;
	switch (zone->dir) {
	case NF_CT_DEFAULT_ZONE_DIR:
		seq_printf(s, "zone=%u ", zone->id);
		break;
	case NF_CT_ZONE_DIR_ORIG:
		seq_printf(s, "zone-orig=%u ", zone->id);
		break;
	case NF_CT_ZONE_DIR_REPL:
		seq_printf(s, "zone-reply=%u ", zone->id);
		break;
	default:
		break;
	}
}