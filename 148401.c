int sldns_str2wire_tsigtime_buf(const char* str, uint8_t* rd, size_t* len)
{
	char* end;
	uint64_t t = (uint64_t)strtol((char*)str, &end, 10);
	uint16_t high;
	uint32_t low;
	if(*end != 0)
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_TIME, end-str);
	if(*len < 6)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	high = (uint16_t)(t>>32);
	low = (uint32_t)(t);
	sldns_write_uint16(rd, high);
	sldns_write_uint32(rd+2, low);
	*len = 6;
	return LDNS_WIREPARSE_ERR_OK;
}