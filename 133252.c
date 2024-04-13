reply_info_copy(struct reply_info* rep, struct alloc_cache* alloc, 
	struct regional* region)
{
	struct reply_info* cp;
	cp = construct_reply_info_base(region, rep->flags, rep->qdcount, 
		rep->ttl, rep->prefetch_ttl, rep->serve_expired_ttl, 
		rep->an_numrrsets, rep->ns_numrrsets, rep->ar_numrrsets,
		rep->rrset_count, rep->security);
	if(!cp)
		return NULL;
	/* allocate ub_key structures special or not */
	if(!reply_info_alloc_rrset_keys(cp, alloc, region)) {
		if(!region)
			reply_info_parsedelete(cp, alloc);
		return NULL;
	}
	if(!repinfo_copy_rrsets(cp, rep, region)) {
		if(!region)
			reply_info_parsedelete(cp, alloc);
		return NULL;
	}
	return cp;
}