reply_check_cname_chain(struct query_info* qinfo, struct reply_info* rep) 
{
	/* check only answer section rrs for matching cname chain.
	 * the cache may return changed rdata, but owner names are untouched.*/
	size_t i;
	uint8_t* sname = qinfo->qname;
	size_t snamelen = qinfo->qname_len;
	for(i=0; i<rep->an_numrrsets; i++) {
		uint16_t t = ntohs(rep->rrsets[i]->rk.type);
		if(t == LDNS_RR_TYPE_DNAME)
			continue; /* skip dnames; note TTL 0 not cached */
		/* verify that owner matches current sname */
		if(query_dname_compare(sname, rep->rrsets[i]->rk.dname) != 0){
			/* cname chain broken */
			return 0;
		}
		/* if this is a cname; move on */
		if(t == LDNS_RR_TYPE_CNAME) {
			get_cname_target(rep->rrsets[i], &sname, &snamelen);
		}
	}
	return 1;
}