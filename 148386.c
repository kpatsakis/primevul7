rrinternal_parse_unknown(sldns_buffer* strbuf, char* token, size_t token_len,
        uint8_t* rr, size_t* rr_len, size_t* rr_cur_len, size_t pre_data_pos)
{
	const char* delim = "\n\t ";
	size_t hex_data_size, cur_hex_data_size;
	/* go back to before \#
	 * and skip it while setting delimiters better
	 */
	sldns_buffer_set_position(strbuf, pre_data_pos);
	if(sldns_bget_token(strbuf, token, delim, token_len) == -1)
		return LDNS_WIREPARSE_ERR_GENERAL; /* should not fail */
	/* read rdata octet length */
	if(sldns_bget_token(strbuf, token, delim, token_len) == -1) {
		/* something goes very wrong here */
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_RDATA,
			sldns_buffer_position(strbuf));
	}
	hex_data_size = (size_t)atoi(token);
	if(hex_data_size > LDNS_MAX_RDFLEN || 
		*rr_cur_len + hex_data_size > *rr_len) {
		return RET_ERR(LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
			sldns_buffer_position(strbuf));
	}
	/* copy hex chars into hex str (2 chars per byte) */
	hex_data_size *= 2;
	cur_hex_data_size = 0;
	while(cur_hex_data_size < hex_data_size) {
		int status;
		ssize_t c = sldns_bget_token(strbuf, token, delim, token_len);
		if((status = rrinternal_spool_hex(token, rr, *rr_len,
			*rr_cur_len, &cur_hex_data_size, hex_data_size)) != 0)
			return RET_ERR_SHIFT(status,
				sldns_buffer_position(strbuf)-strlen(token));
		if(c == -1) {
			if(cur_hex_data_size != hex_data_size)
				return RET_ERR(
					LDNS_WIREPARSE_ERR_SYNTAX_RDATA,
					sldns_buffer_position(strbuf));
			break;
		}
	}
	*rr_cur_len += hex_data_size/2;
	return LDNS_WIREPARSE_ERR_OK;
}