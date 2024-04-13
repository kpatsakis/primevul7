__be16 xfrm_flowi_dport(const struct flowi *fl, const union flowi_uli *uli)
{
	__be16 port;
	switch(fl->flowi_proto) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
	case IPPROTO_UDPLITE:
	case IPPROTO_SCTP:
		port = uli->ports.dport;
		break;
	case IPPROTO_ICMP:
	case IPPROTO_ICMPV6:
		port = htons(uli->icmpt.code);
		break;
	case IPPROTO_GRE:
		port = htons(ntohl(uli->gre_key) & 0xffff);
		break;
	default:
		port = 0;	/*XXX*/
	}
	return port;
}