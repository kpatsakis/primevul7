int sldns_str2wire_int32_buf(const char* str, uint8_t* rd, size_t* len)
{
	char* end;
	uint32_t r;
	errno = 0; /* must set to zero before call,
			note race condition on errno */
	if(*str == '-')
		r = (uint32_t)strtol((char*)str, &end, 10);
	else	r = (uint32_t)strtoul((char*)str, &end, 10);
	if(*end != 0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_INT, end-(char*)str);
	if(errno == ERANGE)
		return LDNS_WIREPARSE_ERR_SYNTAX_INTEGER_OVERFLOW;
	if(*len < 4)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	sldns_write_uint32(rd, r);
	*len = 4;
	return LDNS_WIREPARSE_ERR_OK;
}