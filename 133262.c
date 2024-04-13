parse_create_repinfo(struct msg_parse* msg, struct reply_info** rep,
	struct regional* region)
{
	*rep = construct_reply_info_base(region, msg->flags, msg->qdcount, 0, 
		0, 0, msg->an_rrsets, msg->ns_rrsets, msg->ar_rrsets, 
		msg->rrset_count, sec_status_unchecked);
	if(!*rep)
		return 0;
	return 1;
}