sldns_str2wire_rr_buf_internal(const char* str, uint8_t* rr, size_t* len,
	size_t* dname_len, uint32_t default_ttl, uint8_t* origin,
	size_t origin_len, uint8_t* prev, size_t prev_len, int question)
{
	int status;
	int not_there = 0;
	char token[LDNS_MAX_RDFLEN+1];
	uint32_t ttl = 0;
	uint16_t tp = 0, cl = 0;
	size_t ddlen = 0;

	/* string in buffer */
	sldns_buffer strbuf;
	sldns_buffer_init_frm_data(&strbuf, (uint8_t*)str, strlen(str));
	if(!dname_len) dname_len = &ddlen;

	/* parse the owner */
	if((status=rrinternal_get_owner(&strbuf, rr, len, dname_len, origin,
		origin_len, prev, prev_len, token, sizeof(token))) != 0)
		return status;

	/* parse the [ttl] [class] <type> */
	if((status=rrinternal_get_ttl(&strbuf, token, sizeof(token),
		&not_there, &ttl, default_ttl)) != 0)
		return status;
	if((status=rrinternal_get_class(&strbuf, token, sizeof(token),
		&not_there, &cl)) != 0)
		return status;
	if((status=rrinternal_get_type(&strbuf, token, sizeof(token),
		&not_there, &tp)) != 0)
		return status;
	/* put ttl, class, type into the rr result */
	if((status=rrinternal_write_typeclassttl(&strbuf, rr, *len, *dname_len, tp, cl,
		ttl, question)) != 0)
		return status;
	/* for a question-RR we are done, no rdata */
	if(question) {
		*len = *dname_len + 4;
		return LDNS_WIREPARSE_ERR_OK;
	}

	/* rdata */
	if((status=rrinternal_parse_rdata(&strbuf, token, sizeof(token),
		rr, len, *dname_len, tp, origin, origin_len)) != 0)
		return status;

	return LDNS_WIREPARSE_ERR_OK;
}