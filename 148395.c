sldns_parse_rdf_token(sldns_buffer* strbuf, char* token, size_t token_len,
	int* quoted, int* parens, size_t* pre_data_pos,
	const char* delimiters, sldns_rdf_type rdftype, size_t* token_strlen)
{
	size_t slen;

	/* skip spaces */
	while(sldns_buffer_remaining(strbuf) > 0 && !*quoted &&
		*(sldns_buffer_current(strbuf)) == ' ') {
		sldns_buffer_skip(strbuf, 1);
	}

	*pre_data_pos = sldns_buffer_position(strbuf);
	if(sldns_bget_token_par(strbuf, token, (*quoted)?"\"":delimiters,
		token_len, parens, (*quoted)?NULL:" \t") == -1) {
		return 0;
	}
	slen = strlen(token);
	/* check if not quoted yet, and we have encountered quotes */
	if(!*quoted && sldns_rdf_type_maybe_quoted(rdftype) &&
		slen >= 2 &&
		(token[0] == '"' || token[0] == '\'') && 
		(token[slen-1] == '"' || token[slen-1] == '\'')) {
		/* move token two smaller (quotes) with endnull */
		memmove(token, token+1, slen-2);
		token[slen-2] = 0;
		slen -= 2;
		*quoted = 1;
	} else if(!*quoted && sldns_rdf_type_maybe_quoted(rdftype) &&
		slen >= 2 &&
		(token[0] == '"' || token[0] == '\'')) {
		/* got the start quote (remove it) but read remainder
		 * of quoted string as well into remainder of token */
		memmove(token, token+1, slen-1);
		token[slen-1] = 0;
		slen -= 1;
		*quoted = 1;
		/* rewind buffer over skipped whitespace */
		while(sldns_buffer_position(strbuf) > 0 &&
			(sldns_buffer_current(strbuf)[-1] == ' ' ||
			sldns_buffer_current(strbuf)[-1] == '\t')) {
			sldns_buffer_skip(strbuf, -1);
		}
		if(sldns_bget_token_par(strbuf, token+slen,
			"\"", token_len-slen,
			parens, NULL) == -1) {
			return 0;
		}
		slen = strlen(token);
	}
	*token_strlen = slen;
	return 1;
}