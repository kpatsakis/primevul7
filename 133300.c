reply_info_sortref(struct reply_info* rep)
{
	qsort(&rep->ref[0], rep->rrset_count, sizeof(struct rrset_ref),
		reply_info_sortref_cmp);
}