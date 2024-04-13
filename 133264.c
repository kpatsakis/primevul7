soa_find_minttl(struct rr_parse* rr)
{
	uint16_t rlen = sldns_read_uint16(rr->ttl_data+4);
	if(rlen < 20)
		return 0; /* rdata too small for SOA (dname, dname, 5*32bit) */
	/* minimum TTL is the last 32bit value in the rdata of the record */
	/* at position ttl_data + 4(ttl) + 2(rdatalen) + rdatalen - 4(timeval)*/
	return (time_t)sldns_read_uint32(rr->ttl_data+6+rlen-4);
}