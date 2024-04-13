rrinternal_parse_rdf(sldns_buffer* strbuf, char* token, size_t token_len,
	uint8_t* rr, size_t rr_len, size_t* rr_cur_len, sldns_rdf_type rdftype,
	uint16_t rr_type, size_t r_cnt, size_t r_max, size_t dname_len,
	uint8_t* origin, size_t origin_len)
{
	size_t len;
	int status;

	switch(rdftype) {
	case LDNS_RDF_TYPE_DNAME:
		/* check if the origin should be used or concatenated */
		if(strcmp(token, "@") == 0) {
			uint8_t* tocopy;
			size_t copylen;
			if(origin) {
				copylen = origin_len;
				tocopy = origin;
			} else if(rr_type == LDNS_RR_TYPE_SOA) {
				copylen = dname_len;
				tocopy = rr; /* copy rr owner name */
			} else {
				copylen = 1;
				tocopy = (uint8_t*)"\0";
			}
			if((*rr_cur_len) + copylen > rr_len)
				return RET_ERR(
					LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
					sldns_buffer_position(strbuf));
			memmove(rr+*rr_cur_len, tocopy, copylen);
			(*rr_cur_len) += copylen;
		} else {
			size_t dlen = rr_len - (*rr_cur_len);
			int s = sldns_str2wire_dname_buf_origin(token,
				rr+*rr_cur_len, &dlen, origin, origin_len);
			if(s) return RET_ERR_SHIFT(s,
				sldns_buffer_position(strbuf)-strlen(token));
			(*rr_cur_len) += dlen;
		}
		return LDNS_WIREPARSE_ERR_OK;

	case LDNS_RDF_TYPE_HEX:
	case LDNS_RDF_TYPE_B64:
		/* When this is the last rdata field, then the
		 * rest should be read in (cause then these
		 * rdf types may contain spaces). */
		if(r_cnt == r_max - 1) {
			size_t tlen = strlen(token);
			(void)sldns_bget_token(strbuf, token+tlen, "\n",
				token_len - tlen);
		}
		break;
	default:
		break;
	}

	len = rr_len - (*rr_cur_len);
	if((status=sldns_str2wire_rdf_buf(token, rr+(*rr_cur_len), &len,
		rdftype)) != 0)
		return RET_ERR_SHIFT(status,
			sldns_buffer_position(strbuf)-strlen(token));
	*rr_cur_len += len;
	return LDNS_WIREPARSE_ERR_OK;
}