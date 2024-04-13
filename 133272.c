parse_create_msg(sldns_buffer* pkt, struct msg_parse* msg,
	struct alloc_cache* alloc, struct query_info* qinf, 
	struct reply_info** rep, struct regional* region)
{
	log_assert(pkt && msg);
	if(!parse_create_qinfo(pkt, msg, qinf, region))
		return 0;
	if(!parse_create_repinfo(msg, rep, region))
		return 0;
	if(!reply_info_alloc_rrset_keys(*rep, alloc, region)) {
		if(!region) reply_info_parsedelete(*rep, alloc);
		return 0;
	}
	if(!parse_copy_decompress(pkt, msg, *rep, region)) {
		if(!region) reply_info_parsedelete(*rep, alloc);
		return 0;
	}
	return 1;
}