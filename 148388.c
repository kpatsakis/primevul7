int sldns_str2wire_int16_buf(const char* str, uint8_t* rd, size_t* len)
{
	char* end;
	uint16_t r = (uint16_t)strtol((char*)str, &end, 10);
	if(*end != 0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_INT, end-(char*)str);
	if(*len < 2)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	sldns_write_uint16(rd, r);
	*len = 2;
	return LDNS_WIREPARSE_ERR_OK;
}