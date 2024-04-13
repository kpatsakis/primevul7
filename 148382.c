rrinternal_parse_rdata(sldns_buffer* strbuf, char* token, size_t token_len,
	uint8_t* rr, size_t* rr_len, size_t dname_len, uint16_t rr_type,
	uint8_t* origin, size_t origin_len)
{
	const sldns_rr_descriptor *desc = sldns_rr_descript((uint16_t)rr_type);
	size_t r_cnt, r_min, r_max;
	size_t rr_cur_len = dname_len + 10, pre_data_pos, token_strlen;
	int was_unknown_rr_format = 0, parens = 0, status, quoted;
	const char* delimiters;
	sldns_rdf_type rdftype;
	/* a desc is always returned */
	if(!desc) return LDNS_WIREPARSE_ERR_GENERAL;
	r_max = sldns_rr_descriptor_maximum(desc);
	r_min = sldns_rr_descriptor_minimum(desc);
	/* robust check */
	if(rr_cur_len > *rr_len)
		return RET_ERR(LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
			sldns_buffer_position(strbuf));

	/* because number of fields can be variable, we can't rely on
	 * _maximum() only */
	for(r_cnt=0; r_cnt < r_max; r_cnt++) {
		rdftype = sldns_rr_descriptor_field_type(desc, r_cnt);
		delimiters = rrinternal_get_delims(rdftype, r_cnt, r_max);
		quoted = rrinternal_get_quoted(strbuf, &delimiters, rdftype);

		if(!sldns_parse_rdf_token(strbuf, token, token_len, &quoted,
			&parens, &pre_data_pos, delimiters, rdftype,
			&token_strlen))
			break;

		/* rfc3597 specifies that any type can be represented
		 * with \# method, which can contain spaces...
		 * it does specify size though... */

		/* unknown RR data */
		if(token_strlen>=2 && strncmp(token, "\\#", 2) == 0 &&
			!quoted && (token_strlen == 2 || token[2]==' ')) {
			was_unknown_rr_format = 1;
			if((status=rrinternal_parse_unknown(strbuf, token,
				token_len, rr, rr_len, &rr_cur_len, 
				pre_data_pos)) != 0)
				return status;
		} else if(token_strlen > 0 || quoted) {
			if(rdftype == LDNS_RDF_TYPE_HIP) {
				/* affix the HIT and PK fields, with a space */
				if(!sldns_affix_token(strbuf, token,
					&token_len, &quoted, &parens,
					&pre_data_pos, delimiters,
					rdftype, &token_strlen))
					break;
				if(!sldns_affix_token(strbuf, token,
					&token_len, &quoted, &parens,
					&pre_data_pos, delimiters,
					rdftype, &token_strlen))
					break;
			} else if(rdftype == LDNS_RDF_TYPE_INT16_DATA &&
				strcmp(token, "0")!=0) {
				/* affix len and b64 fields */
				if(!sldns_affix_token(strbuf, token,
					&token_len, &quoted, &parens,
					&pre_data_pos, delimiters,
					rdftype, &token_strlen))
					break;
			}

			/* normal RR */
			if((status=rrinternal_parse_rdf(strbuf, token,
				token_len, rr, *rr_len, &rr_cur_len, rdftype,
				rr_type, r_cnt, r_max, dname_len, origin,
				origin_len)) != 0) {
				return status;
			}
		}
	}
	if(!was_unknown_rr_format && r_cnt+1 < r_min) {
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_MISSING_VALUE,
			sldns_buffer_position(strbuf));
	}
	while(parens != 0) {
		/* read remainder, must be "" */
		if(sldns_bget_token_par(strbuf, token, "\n", token_len,
			&parens, " \t") == -1) {
			if(parens != 0)
				return RET_ERR(LDNS_WIREPARSE_ERR_PARENTHESIS,
					sldns_buffer_position(strbuf));
			break;
		}
		if(strcmp(token, "") != 0)
			return RET_ERR(LDNS_WIREPARSE_ERR_PARENTHESIS,
				sldns_buffer_position(strbuf));
	}
	/* write rdata length */
	sldns_write_uint16(rr+dname_len+8, (uint16_t)(rr_cur_len-dname_len-10));
	*rr_len = rr_cur_len;
	return LDNS_WIREPARSE_ERR_OK;
}