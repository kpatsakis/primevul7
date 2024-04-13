int sldns_str2wire_int8_buf(const char* str, uint8_t* rd, size_t* len)
{
	char* end;
	uint8_t r = (uint8_t)strtol((char*)str, &end, 10);
	if(*end != 0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_INT, end-(char*)str);
	if(*len < 1)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	rd[0] = r;
	*len = 1;
	return LDNS_WIREPARSE_ERR_OK;
}