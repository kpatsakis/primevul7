reply_info_set_ttls(struct reply_info* rep, time_t timenow)
{
	size_t i, j;
	rep->ttl += timenow;
	rep->prefetch_ttl += timenow;
	rep->serve_expired_ttl += timenow;
	for(i=0; i<rep->rrset_count; i++) {
		struct packed_rrset_data* data = (struct packed_rrset_data*)
			rep->ref[i].key->entry.data;
		if(i>0 && rep->ref[i].key == rep->ref[i-1].key)
			continue;
		data->ttl += timenow;
		for(j=0; j<data->count + data->rrsig_count; j++) {
			data->rr_ttl[j] += timenow;
		}
	}
}