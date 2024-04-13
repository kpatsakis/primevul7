log_query_info(enum verbosity_value v, const char* str, 
	struct query_info* qinf)
{
	log_nametypeclass(v, str, qinf->qname, qinf->qtype, qinf->qclass);
}