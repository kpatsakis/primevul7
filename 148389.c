int sldns_str2wire_nsap_buf(const char* str, uint8_t* rd, size_t* len)
{
	const char* s = str;
	size_t slen;
	size_t dlen = 0; /* number of hexdigits parsed */

	/* just a hex string with optional dots? */
	if (s[0] != '0' || s[1] != 'x')
		return LDNS_WIREPARSE_ERR_INVALID_STR;
	s += 2;
	slen = strlen(s);
	if(slen > LDNS_MAX_RDFLEN*2)
		return LDNS_WIREPARSE_ERR_LABEL_OVERFLOW;
	while(*s) {
		if(isspace((unsigned char)*s) || *s == '.') {
			s++;
			continue;
		}
		if(!isxdigit((unsigned char)*s))
			return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_HEX, s-str);
		if(*len < dlen/2 + 1)
			return RET_ERR(LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
				s-str);
		if((dlen&1)==0)
			rd[dlen/2] = (uint8_t)sldns_hexdigit_to_int(*s++) * 16;
		else	rd[dlen/2] += sldns_hexdigit_to_int(*s++);
		dlen++;
	}
	if((dlen&1)!=0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_HEX, s-str);
	*len = dlen/2;
	return LDNS_WIREPARSE_ERR_OK;
}