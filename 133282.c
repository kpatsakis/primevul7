query_info_hash(struct query_info *q, uint16_t flags)
{
	hashvalue_type h = 0xab;
	h = hashlittle(&q->qtype, sizeof(q->qtype), h);
	if(q->qtype == LDNS_RR_TYPE_AAAA && (flags&BIT_CD))
		h++;
	h = hashlittle(&q->qclass, sizeof(q->qclass), h);
	h = dname_query_hash(q->qname, h);
	return h;
}