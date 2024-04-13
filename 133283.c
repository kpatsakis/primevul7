parse_create_qinfo(sldns_buffer* pkt, struct msg_parse* msg, 
	struct query_info* qinf, struct regional* region)
{
	if(msg->qname) {
		if(region)
			qinf->qname = (uint8_t*)regional_alloc(region, 
				msg->qname_len);
		else	qinf->qname = (uint8_t*)malloc(msg->qname_len);
		if(!qinf->qname) return 0;
		dname_pkt_copy(pkt, qinf->qname, msg->qname);
	} else	qinf->qname = 0;
	qinf->qname_len = msg->qname_len;
	qinf->qtype = msg->qtype;
	qinf->qclass = msg->qclass;
	qinf->local_alias = NULL;
	return 1;
}