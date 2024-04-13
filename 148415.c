int sldns_str2wire_alg_buf(const char* str, uint8_t* rd, size_t* len)
{
	sldns_lookup_table *lt = sldns_lookup_by_name(sldns_algorithms, str);
	if(*len < 1)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	if(lt) {
		rd[0] = (uint8_t)lt->id;
		*len = 1;
	} else {
		/* try as-is (a number) */
		return sldns_str2wire_int8_buf(str, rd, len);
	}
	return LDNS_WIREPARSE_ERR_OK;
}