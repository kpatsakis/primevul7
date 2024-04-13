int sldns_str2wire_b32_ext_buf(const char* str, uint8_t* rd, size_t* len)
{
	size_t slen = strlen(str);
	size_t sz = sldns_b32_pton_calculate_size(slen);
	int n;
	if(*len < 1+sz)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	rd[0] = (uint8_t)sz;
	n = sldns_b32_pton_extended_hex(str, slen, rd+1, *len-1);
	if(n < 0)
		return LDNS_WIREPARSE_ERR_SYNTAX_B32_EXT;
	*len = (size_t)n+1;
	return LDNS_WIREPARSE_ERR_OK;
}