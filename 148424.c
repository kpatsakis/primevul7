int sldns_str2wire_tsigerror_buf(const char* str, uint8_t* rd, size_t* len)
{
	sldns_lookup_table *lt = sldns_lookup_by_name(sldns_tsig_errors, str);
	if(*len < 2)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	if(lt) {
		sldns_write_uint16(rd, (uint16_t)lt->id);
		*len = 2;
	} else {
		/* try as-is (a number) */
		return sldns_str2wire_int16_buf(str, rd, len);
	}
	return LDNS_WIREPARSE_ERR_OK;
}