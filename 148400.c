rrinternal_get_quoted(sldns_buffer* strbuf, const char** delimiters,
	sldns_rdf_type rdftype)
{
	if(sldns_rdf_type_maybe_quoted(rdftype) &&
		sldns_buffer_remaining(strbuf) > 0) {

		/* skip spaces */
		while(sldns_buffer_remaining(strbuf) > 0 &&
			*(sldns_buffer_current(strbuf)) == ' ') {
			sldns_buffer_skip(strbuf, 1);
		}

		if(sldns_buffer_remaining(strbuf) > 0 &&
			*(sldns_buffer_current(strbuf)) == '\"') {
			*delimiters = "\"\0";
			sldns_buffer_skip(strbuf, 1);
			return 1;
		}
	}
	return 0;
}