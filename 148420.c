rrinternal_get_type(sldns_buffer* strbuf, char* token, size_t token_len,
	int* not_there, uint16_t* tp)
{
	/* if 'not_there' then we got token from previous parse routine */
	if(!*not_there) {
		/* parse new token for type */
		if(sldns_bget_token(strbuf, token, "\t\n ", token_len) == -1) {
			return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_TYPE,
				sldns_buffer_position(strbuf));
		}
	}
	*tp = sldns_get_rr_type_by_name(token);
	if(*tp == 0 && strcmp(token, "TYPE0") != 0) {
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_TYPE,
			sldns_buffer_position(strbuf));
	}
	return LDNS_WIREPARSE_ERR_OK;
}