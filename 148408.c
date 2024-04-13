int sldns_str2wire_b64_buf(const char* str, uint8_t* rd, size_t* len)
{
	size_t sz = sldns_b64_pton_calculate_size(strlen(str));
	int n;
	if(strcmp(str, "0") == 0) {
		*len = 0;
		return LDNS_WIREPARSE_ERR_OK;
	}
	if(*len < sz)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	n = sldns_b64_pton(str, rd, *len);
	if(n < 0)
		return LDNS_WIREPARSE_ERR_SYNTAX_B64;
	*len = (size_t)n;
	return LDNS_WIREPARSE_ERR_OK;
}