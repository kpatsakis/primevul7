int sldns_str2wire_hex_buf(const char* str, uint8_t* rd, size_t* len)
{
	const char* s = str;
	size_t dlen = 0; /* number of hexdigits parsed */
	while(*s) {
		if(isspace((unsigned char)*s)) {
			s++;
			continue;
		}
		if(dlen == 0 && *s == '0' && sldns_is_last_of_string(s+1)) {
			*len = 0;
			return LDNS_WIREPARSE_ERR_OK;
		}
		if(!isxdigit((unsigned char)*s))
			return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_HEX, s-str);
		if(*len < dlen/2 + 1)
			return RET_ERR(LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
				s-str);
		if((dlen&1)==0)
			rd[dlen/2] = (uint8_t)sldns_hexdigit_to_int(*s++) * 16;
		else	rd[dlen/2] += (uint8_t)sldns_hexdigit_to_int(*s++);
		dlen++;
	}
	if((dlen&1)!=0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_HEX, s-str);
	*len = dlen/2;
	return LDNS_WIREPARSE_ERR_OK;
}