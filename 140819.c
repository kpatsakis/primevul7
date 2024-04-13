print_tuple(struct seq_file *s, const struct nf_conntrack_tuple *tuple,
            const struct nf_conntrack_l4proto *l4proto)
{
	switch (tuple->src.l3num) {
	case NFPROTO_IPV4:
		seq_printf(s, "src=%pI4 dst=%pI4 ",
			   &tuple->src.u3.ip, &tuple->dst.u3.ip);
		break;
	case NFPROTO_IPV6:
		seq_printf(s, "src=%pI6 dst=%pI6 ",
			   tuple->src.u3.ip6, tuple->dst.u3.ip6);
		break;
	default:
		break;
	}

	switch (l4proto->l4proto) {
	case IPPROTO_ICMP:
		seq_printf(s, "type=%u code=%u id=%u ",
			   tuple->dst.u.icmp.type,
			   tuple->dst.u.icmp.code,
			   ntohs(tuple->src.u.icmp.id));
		break;
	case IPPROTO_TCP:
		seq_printf(s, "sport=%hu dport=%hu ",
			   ntohs(tuple->src.u.tcp.port),
			   ntohs(tuple->dst.u.tcp.port));
		break;
	case IPPROTO_UDPLITE:
	case IPPROTO_UDP:
		seq_printf(s, "sport=%hu dport=%hu ",
			   ntohs(tuple->src.u.udp.port),
			   ntohs(tuple->dst.u.udp.port));

		break;
	case IPPROTO_DCCP:
		seq_printf(s, "sport=%hu dport=%hu ",
			   ntohs(tuple->src.u.dccp.port),
			   ntohs(tuple->dst.u.dccp.port));
		break;
	case IPPROTO_SCTP:
		seq_printf(s, "sport=%hu dport=%hu ",
			   ntohs(tuple->src.u.sctp.port),
			   ntohs(tuple->dst.u.sctp.port));
		break;
	case IPPROTO_ICMPV6:
		seq_printf(s, "type=%u code=%u id=%u ",
			   tuple->dst.u.icmp.type,
			   tuple->dst.u.icmp.code,
			   ntohs(tuple->src.u.icmp.id));
		break;
	case IPPROTO_GRE:
		seq_printf(s, "srckey=0x%x dstkey=0x%x ",
			   ntohs(tuple->src.u.gre.key),
			   ntohs(tuple->dst.u.gre.key));
		break;
	default:
		break;
	}
}