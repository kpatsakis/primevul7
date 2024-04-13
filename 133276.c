repinfo_copy_rrsets(struct reply_info* dest, struct reply_info* from, 
	struct regional* region)
{
	size_t i, s;
	struct packed_rrset_data* fd, *dd;
	struct ub_packed_rrset_key* fk, *dk;
	for(i=0; i<dest->rrset_count; i++) {
		fk = from->rrsets[i];
		dk = dest->rrsets[i];
		fd = (struct packed_rrset_data*)fk->entry.data;
		dk->entry.hash = fk->entry.hash;
		dk->rk = fk->rk;
		if(region) {
			dk->id = fk->id;
			dk->rk.dname = (uint8_t*)regional_alloc_init(region,
				fk->rk.dname, fk->rk.dname_len);
		} else	
			dk->rk.dname = (uint8_t*)memdup(fk->rk.dname, 
				fk->rk.dname_len);
		if(!dk->rk.dname)
			return 0;
		s = packed_rrset_sizeof(fd);
		if(region)
			dd = (struct packed_rrset_data*)regional_alloc_init(
				region, fd, s);
		else	dd = (struct packed_rrset_data*)memdup(fd, s);
		if(!dd) 
			return 0;
		packed_rrset_ptr_fixup(dd);
		dk->entry.data = (void*)dd;
	}
	return 1;
}