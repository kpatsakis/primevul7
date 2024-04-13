int sldns_str2wire_class_buf(const char* str, uint8_t* rd, size_t* len)
{
	uint16_t c = sldns_get_rr_class_by_name(str);
	if(c == 0 && strcmp(str, "CLASS0") != 0)
		return LDNS_WIREPARSE_ERR_SYNTAX_CLASS;
	if(*len < 2)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	sldns_write_uint16(rd, c);
	*len = 2;
	return LDNS_WIREPARSE_ERR_OK;
}