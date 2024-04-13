sldns_affix_token(sldns_buffer* strbuf, char* token, size_t* token_len,
	int* quoted, int* parens, size_t* pre_data_pos,
	const char* delimiters, sldns_rdf_type rdftype, size_t* token_strlen)
{
	size_t addlen = *token_len - *token_strlen;
	size_t addstrlen = 0;

	/* add space */
	if(addlen < 1) return 0;
	token[*token_strlen] = ' ';
	token[++(*token_strlen)] = 0;

	/* read another token */
	addlen = *token_len - *token_strlen;
	if(!sldns_parse_rdf_token(strbuf, token+*token_strlen, addlen, quoted,
		parens, pre_data_pos, delimiters, rdftype, &addstrlen))
		return 0;
	(*token_strlen) += addstrlen;
	return 1;
}