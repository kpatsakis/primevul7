reply_all_rrsets_secure(struct reply_info* rep) 
{
	size_t i;
	for(i=0; i<rep->rrset_count; i++) {
		if( ((struct packed_rrset_data*)rep->rrsets[i]->entry.data)
			->security != sec_status_secure )
		return 0;
	}
	return 1;
}