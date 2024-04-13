parse_copy_decompress(sldns_buffer* pkt, struct msg_parse* msg,
	struct reply_info* rep, struct regional* region)
{
	size_t i;
	struct rrset_parse *pset = msg->rrset_first;
	struct packed_rrset_data* data;
	log_assert(rep);
	rep->ttl = MAX_TTL;
	rep->security = sec_status_unchecked;
	if(rep->rrset_count == 0)
		rep->ttl = NORR_TTL;

	for(i=0; i<rep->rrset_count; i++) {
		if(!parse_copy_decompress_rrset(pkt, msg, pset, region,
			rep->rrsets[i]))
			return 0;
		data = (struct packed_rrset_data*)rep->rrsets[i]->entry.data;
		if(data->ttl < rep->ttl)
			rep->ttl = data->ttl;

		pset = pset->rrset_all_next;
	}
	rep->prefetch_ttl = PREFETCH_TTL_CALC(rep->ttl);
	rep->serve_expired_ttl = rep->ttl + SERVE_EXPIRED_TTL;
	return 1;
}