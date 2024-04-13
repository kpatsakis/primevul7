parse_reply_in_temp_region(sldns_buffer* pkt, struct regional* region,
	struct query_info* qi)
{
	struct reply_info* rep;
	struct msg_parse* msg;
	if(!(msg = regional_alloc(region, sizeof(*msg)))) {
		return NULL;
	}
	memset(msg, 0, sizeof(*msg));
	sldns_buffer_set_position(pkt, 0);
	if(parse_packet(pkt, msg, region) != 0){
		return 0;
	}
	if(!parse_create_msg(pkt, msg, NULL, qi, &rep, region)) {
		return 0;
	}
	return rep;
}