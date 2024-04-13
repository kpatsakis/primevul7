int sldns_str2wire_period_buf(const char* str, uint8_t* rd, size_t* len)
{
	const char* end;
	uint32_t p = sldns_str2period(str, &end);
	if(*end != 0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_PERIOD, end-str);
	if(*len < 4)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	sldns_write_uint32(rd, p);
	*len = 4;
	return LDNS_WIREPARSE_ERR_OK;
}