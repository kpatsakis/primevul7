parse_create_rrset(sldns_buffer* pkt, struct rrset_parse* pset,
	struct packed_rrset_data** data, struct regional* region)
{
	/* allocate */
	size_t s;
	if(pset->rr_count > RR_COUNT_MAX || pset->rrsig_count > RR_COUNT_MAX ||
		pset->size > RR_COUNT_MAX)
		return 0; /* protect against integer overflow */
	s = sizeof(struct packed_rrset_data) + 
		(pset->rr_count + pset->rrsig_count) * 
		(sizeof(size_t)+sizeof(uint8_t*)+sizeof(time_t)) + 
		pset->size;
	if(region)
		*data = regional_alloc(region, s);
	else	*data = malloc(s);
	if(!*data)
		return 0;
	/* copy & decompress */
	if(!parse_rr_copy(pkt, pset, *data)) {
		if(!region) free(*data);
		return 0;
	}
	return 1;
}