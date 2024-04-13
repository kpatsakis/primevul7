query_info_parse(struct query_info* m, sldns_buffer* query)
{
	uint8_t* q = sldns_buffer_begin(query);
	/* minimum size: header + \0 + qtype + qclass */
	if(sldns_buffer_limit(query) < LDNS_HEADER_SIZE + 5)
		return 0;
	if((LDNS_OPCODE_WIRE(q) != LDNS_PACKET_QUERY && LDNS_OPCODE_WIRE(q) !=
		LDNS_PACKET_NOTIFY) || LDNS_QDCOUNT(q) != 1 ||
		sldns_buffer_position(query) != 0)
		return 0;
	sldns_buffer_skip(query, LDNS_HEADER_SIZE);
	m->qname = sldns_buffer_current(query);
	if((m->qname_len = query_dname_len(query)) == 0)
		return 0; /* parse error */
	if(sldns_buffer_remaining(query) < 4)
		return 0; /* need qtype, qclass */
	m->qtype = sldns_buffer_read_u16(query);
	m->qclass = sldns_buffer_read_u16(query);
	m->local_alias = NULL;
	return 1;
}