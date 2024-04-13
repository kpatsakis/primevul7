query_info_entrysetup(struct query_info* q, struct reply_info* r, 
	hashvalue_type h)
{
	struct msgreply_entry* e = (struct msgreply_entry*)malloc( 
		sizeof(struct msgreply_entry));
	if(!e) return NULL;
	memcpy(&e->key, q, sizeof(*q));
	e->entry.hash = h;
	e->entry.key = e;
	e->entry.data = r;
	lock_rw_init(&e->entry.lock);
	lock_protect(&e->entry.lock, &e->key.qname, sizeof(e->key.qname));
	lock_protect(&e->entry.lock, &e->key.qname_len, sizeof(e->key.qname_len));
	lock_protect(&e->entry.lock, &e->key.qtype, sizeof(e->key.qtype));
	lock_protect(&e->entry.lock, &e->key.qclass, sizeof(e->key.qclass));
	lock_protect(&e->entry.lock, &e->key.local_alias, sizeof(e->key.local_alias));
	lock_protect(&e->entry.lock, &e->entry.hash, sizeof(e->entry.hash));
	lock_protect(&e->entry.lock, &e->entry.key, sizeof(e->entry.key));
	lock_protect(&e->entry.lock, &e->entry.data, sizeof(e->entry.data));
	lock_protect(&e->entry.lock, e->key.qname, e->key.qname_len);
	q->qname = NULL;
	return e;
}