parse_copy_decompress_rrset(sldns_buffer* pkt, struct msg_parse* msg,
	struct rrset_parse *pset, struct regional* region, 
	struct ub_packed_rrset_key* pk)
{
	struct packed_rrset_data* data;
	pk->rk.flags = pset->flags;
	pk->rk.dname_len = pset->dname_len;
	if(region)
		pk->rk.dname = (uint8_t*)regional_alloc(
			region, pset->dname_len);
	else	pk->rk.dname = 
			(uint8_t*)malloc(pset->dname_len);
	if(!pk->rk.dname)
		return 0;
	/** copy & decompress dname */
	dname_pkt_copy(pkt, pk->rk.dname, pset->dname);
	/** copy over type and class */
	pk->rk.type = htons(pset->type);
	pk->rk.rrset_class = pset->rrset_class;
	/** read data part. */
	if(!parse_create_rrset(pkt, pset, &data, region))
		return 0;
	pk->entry.data = (void*)data;
	pk->entry.key = (void*)pk;
	pk->entry.hash = pset->hash;
	data->trust = get_rrset_trust(msg, pset);
	return 1;
}