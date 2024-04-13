int sldns_str2wire_type_buf(const char* str, uint8_t* rd, size_t* len)
{
	uint16_t t = sldns_get_rr_type_by_name(str);
	if(t == 0 && strcmp(str, "TYPE0") != 0)
		return LDNS_WIREPARSE_ERR_SYNTAX_TYPE;
	if(*len < 2)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	sldns_write_uint16(rd, t);
	*len = 2;
	return LDNS_WIREPARSE_ERR_OK;
}