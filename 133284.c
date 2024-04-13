reply_info_alloc_rrset_keys(struct reply_info* rep, struct alloc_cache* alloc,
	struct regional* region)
{
	size_t i;
	for(i=0; i<rep->rrset_count; i++) {
		if(region) {
			rep->rrsets[i] = (struct ub_packed_rrset_key*)
				regional_alloc(region, 
				sizeof(struct ub_packed_rrset_key));
			if(rep->rrsets[i]) {
				memset(rep->rrsets[i], 0, 
					sizeof(struct ub_packed_rrset_key));
				rep->rrsets[i]->entry.key = rep->rrsets[i];
			}
		}
		else	rep->rrsets[i] = alloc_special_obtain(alloc);
		if(!rep->rrsets[i])
			return 0;
		rep->rrsets[i]->entry.data = NULL;
	}
	return 1;
}