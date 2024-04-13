msgreply_sizefunc(void* k, void* d)
{
	struct msgreply_entry* q = (struct msgreply_entry*)k;
	struct reply_info* r = (struct reply_info*)d;
	size_t s = sizeof(struct msgreply_entry) + sizeof(struct reply_info)
		+ q->key.qname_len + lock_get_mem(&q->entry.lock)
		- sizeof(struct rrset_ref);
	s += r->rrset_count * sizeof(struct rrset_ref);
	s += r->rrset_count * sizeof(struct ub_packed_rrset_key*);
	return s;
}